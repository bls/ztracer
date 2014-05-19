#include "TraceMalloc.h"
#include "Log.h"
#include "ZTracer.h"

LOG_INIT("TraceMalloc");

#define MALLOC "malloc"
#define FREE "free"
#define CALLOC "calloc"
#define REALLOC "realloc"

#define START_HOOK "__libc_start_main"

// TODO: See http://tech.groups.yahoo.com/group/pinheads/message/7765
// IPOINT_AFTER doesn't succeed all the time, and in particular, it doesn't
// work too well for the malloc used in the dynamic loader.
// This is why we instrument malloc() by wrapping it, rather than before/after.

static UINT32 mallocIndex = 0;
static UINT32 freeIndex = 0;
static UINT32 reallocIndex = 0;
static UINT32 callocIndex = 0;

// Only log calls after app startup to reduce noise

static bool start_seen = 0;


VOID * MallocWrapper(CONTEXT * ctxt, AFUNPTR pf_malloc, UINT32 idx, size_t size, ZTracer *ztracer);
VOID * CallocWrapper(CONTEXT * ctxt, AFUNPTR pf_calloc, UINT32 idx, size_t size, ZTracer *ztracer);
VOID * ReallocWrapper(CONTEXT * ctxt, AFUNPTR pf_realloc, UINT32 idx, ADDRINT p, size_t size, ZTracer *ztracer);
VOID FreeBefore(ADDRINT retIp, UINT32 freeIndex, ADDRINT p, ZTracer *ztracer);
VOID MainBefore();

VOID ImgTraceMalloc(IMG img, VOID *v)
{
    void *ztracer = v;
    
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().

    std::string img_name = IMG_Name(img);
    
    if(img_name.find("ld-linux") != std::string::npos) {
        return; // Do not instrument malloc/free from ld-linux
    }

    // Patch malloc (wrapper)
    RTN mallocRtn = RTN_FindByName(img, MALLOC);  
    if (RTN_Valid(mallocRtn))
    {
        PROTO protoMalloc = PROTO_Allocate( PIN_PARG(void *), CALLINGSTD_DEFAULT, MALLOC,
                                            PIN_PARG(size_t), PIN_PARG(void *), PIN_PARG_END() );

        RTN_ReplaceSignature(mallocRtn, AFUNPTR(MallocWrapper),
                             IARG_PROTOTYPE, protoMalloc,
                             IARG_CONST_CONTEXT,
                             IARG_ORIG_FUNCPTR,
                             IARG_UINT32, mallocIndex,                             
                             IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                             IARG_PTR, ztracer,
                             IARG_END);
        LOG_INFO("Instrumented %s() in image %s (idx %d)", MALLOC, img_name.c_str(), mallocIndex);
        mallocIndex++;
    }

    // Patch calloc (wrapper)
    RTN callocRtn = RTN_FindByName(img, CALLOC);  
    if (RTN_Valid(callocRtn))
    {
        PROTO protoCalloc = PROTO_Allocate( PIN_PARG(void *), CALLINGSTD_DEFAULT, CALLOC,
                                            PIN_PARG(size_t), PIN_PARG(void *), PIN_PARG_END() );

        RTN_ReplaceSignature(callocRtn, AFUNPTR(CallocWrapper),
                             IARG_PROTOTYPE, protoCalloc,
                             IARG_CONST_CONTEXT,
                             IARG_ORIG_FUNCPTR,
                             IARG_UINT32, callocIndex,                      
                             IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                             IARG_PTR, ztracer,
                             IARG_END);
        LOG_INFO("Instrumented %s() in image %s (idx %d)", CALLOC, img_name.c_str(), callocIndex);
        callocIndex++;
    }

    // Patch realloc (wrapper)
    RTN reallocRtn = RTN_FindByName(img, REALLOC);  
    if (RTN_Valid(reallocRtn))
    {
        PROTO protoRealloc = PROTO_Allocate( PIN_PARG(void *), CALLINGSTD_DEFAULT, REALLOC,
                                             PIN_PARG(void *), PIN_PARG(size_t), PIN_PARG(void *), PIN_PARG_END() );

        RTN_ReplaceSignature(reallocRtn, AFUNPTR(ReallocWrapper),
                             IARG_PROTOTYPE, protoRealloc,
                             IARG_CONST_CONTEXT,
                             IARG_ORIG_FUNCPTR,
                             IARG_UINT32, reallocIndex,
                             IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                             IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                             IARG_PTR, ztracer,
                             IARG_END);
        LOG_INFO("Instrumented %s() in image %s (idx %d)", REALLOC, img_name.c_str(), reallocIndex);
        reallocIndex++;
    }

    // Instrument free (before hook)
    RTN freeRtn = RTN_FindByName(img, FREE); 
    if (RTN_Valid(freeRtn))
    {
        RTN_Open(freeRtn);
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)FreeBefore,
                       IARG_RETURN_IP,
                       IARG_UINT32, freeIndex,        
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_PTR, ztracer,
                       IARG_END);
        RTN_Close(freeRtn);
        LOG_INFO("Instrumented %s() in image %s (idx %d)", FREE, img_name.c_str(), freeIndex);
        freeIndex++;
    }

    // Start hook. Would be nice to do main(), but that would be a fair bit of work
    // to do without symbols. Use __libc_start_main, it's a lot less work.
    RTN mainRtn = RTN_FindByName(img, START_HOOK); 
    if (RTN_Valid(mainRtn)) {
        RTN_Open(mainRtn);
        RTN_InsertCall(mainRtn, IPOINT_BEFORE, (AFUNPTR)MainBefore, IARG_END);
        RTN_Close(mainRtn);
        LOG_INFO("Instrumented %s() in image %s", START_HOOK, img_name.c_str());
    }

}

VOID MainBefore()
{
    LOG_INFO("__libc_start_main() entry");
    start_seen = 1;    
}

VOID FreeBefore(ADDRINT retIp, UINT32 freeIndex, ADDRINT p, ZTracer *ztracer)
{
    if(start_seen) {
        if(p) { // Let's not clutter up the logs
            LOG_DEBUG("free(0x%x) [idx %d]", p, freeIndex);
            ztrace::Event ev = ztracer->MakeEvent();
            ztrace::Free *freeEv = ev.mutable_free();
            freeEv->set_p(p);
            ztracer->WriteEvent(&ev);
        }
    }
}

VOID * MallocWrapper(CONTEXT * ctxt, AFUNPTR pf_malloc, UINT32 idx, size_t size, ZTracer *ztracer)
{  
    ADDRINT res;
    PIN_CallApplicationFunction(ctxt, PIN_ThreadId(),
                                CALLINGSTD_DEFAULT,  pf_malloc,
                                PIN_PARG(ADDRINT),  &res,
                                PIN_PARG(size_t), size,
                                PIN_PARG_END());
    if(start_seen) {
        LOG_DEBUG("malloc(%d) => %p [idx %d]", size, res, idx);
        ztrace::Event ev = ztracer->MakeEvent();
        ztrace::Malloc *mallocEv = ev.mutable_malloc();
        mallocEv->set_n(size);
        mallocEv->set_ret(res);
        ztracer->WriteEvent(&ev);
    }
    return reinterpret_cast<VOID *>(res);
}


VOID * CallocWrapper(CONTEXT * ctxt, AFUNPTR pf_calloc, UINT32 idx, size_t size, ZTracer *ztracer)
{  
    ADDRINT res;
    PIN_CallApplicationFunction(ctxt, PIN_ThreadId(),
                                CALLINGSTD_DEFAULT,  pf_calloc,
                                PIN_PARG(ADDRINT),  &res,
                                PIN_PARG(size_t), size,
                                PIN_PARG_END());
    if(start_seen) {
        LOG_DEBUG("calloc(%d) => %p [idx %d]", size, res, idx);
        ztrace::Event ev = ztracer->MakeEvent();
        ztrace::Calloc *callocEv = ev.mutable_calloc();
        callocEv->set_n(size);
        callocEv->set_ret(res);
        ztracer->WriteEvent(&ev);
    }
    return reinterpret_cast<VOID *>(res);
}


VOID * ReallocWrapper(CONTEXT * ctxt, AFUNPTR pf_realloc, UINT32 idx, ADDRINT p, size_t size, ZTracer *ztracer)
{  
    ADDRINT res;
    PIN_CallApplicationFunction(ctxt, PIN_ThreadId(),
                                CALLINGSTD_DEFAULT,  pf_realloc,
                                PIN_PARG(ADDRINT), &res,
                                PIN_PARG(ADDRINT), p,
                                PIN_PARG(size_t), size,
                                PIN_PARG_END());
    if(start_seen) {
        LOG_DEBUG("realloc(0x%x, %d) => %p [idx %d]", p, size, res, idx);
        ztrace::Event ev = ztracer->MakeEvent();
        ztrace::Realloc *reallocEv = ev.mutable_realloc();
        reallocEv->set_n(size);
        reallocEv->set_p(p);
        reallocEv->set_ret(res);
    }
    return reinterpret_cast<VOID *>(res);
}
