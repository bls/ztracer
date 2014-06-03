#include "ZTracer.h"
#include "Log.h"
#include "Helpers.h"
#include "ThreadData.h"
#include "MicroEventLog.h"
#include "TraceDisassembly.h"
#include "TraceMicroEvent.h"
#include "TraceRegisters.h"
#include "TraceModules.h"
#include "TraceMalloc.h"
#include "MemoryMap.h"
#include <sys/time.h>
#include <arpa/inet.h>
#include <limits>
#include <vector>

LOG_INIT("ZTracer");

class PIN_Trampoline
{
public:
    static VOID ApplicationStart(VOID *v)
    { static_cast<ZTracer *>(v)->OnApplicationStart(); }
    
    static VOID Finish(INT32 code, VOID *v)
    { static_cast<ZTracer *>(v)->OnFinish(code); }

    static BOOL FollowChildProcess(CHILD_PROCESS childProcess, VOID *v)
    { return static_cast<ZTracer *>(v)->OnFollowChildProcess(childProcess); }

    static VOID ThreadStart(THREADID threadIndex, CONTEXT *ctx, INT32 flags, VOID *v)
    { static_cast<ZTracer *>(v)->OnThreadStart(threadIndex, ctx, flags); }

    static VOID ThreadFinish(THREADID threadIndex, const CONTEXT *ctx, INT32 flags, VOID *v)
    { static_cast<ZTracer *>(v)->OnThreadFinish(threadIndex, ctx, flags); }
};

ZTracer::ZTracer()
    : event_id_(0)
{
    PIN_InitLock(&output_lock_);
    PIN_InitLock(&thread_state_lock_);
    ThreadData::Init(); // TODO: messy to do this here.
}

bool
ZTracer::OpenLog(std::string path)
{
    output_stream_.open(path.c_str(), std::ofstream::binary);
    if(output_stream_.fail()) {
        return false;
    }
    LogTraceStart();
    return true;
}

void
ZTracer::CloseLog()
{
    if(output_stream_.is_open()) {
        LogTraceEnd();        
        output_stream_.close();
    }
}

ztrace::Event
ZTracer::MakeEvent()
{
    ztrace::Event ev;
    // Event number is not filled out until just before write.
    ev.set_pid( PIN_GetPid() );
    ev.set_tid( PIN_GetTid() );
    return ev;
}

void
ZTracer::WriteEvent(ztrace::Event *msg)
{
    PIN_GetLock(&output_lock_, PIN_GetTid() );

    // Annotate event with cross references to live MicroEventLogs
    // We hold a thread state lock here because we don't want any changes
    // to the live thread list while we do this.
    PIN_GetLock(&thread_state_lock_, PIN_GetTid());
    std::set<THREADID>::iterator it;
    for(it = live_threads_.begin(); it != live_threads_.end(); ++it) {
        THREADID tid = *it;
        MicroEventLog *microEventLog = ThreadData::get_micro_event_log(tid);
        UINT64 nextIns = microEventLog->getNextInstructionIndex();
        ztrace::MicroEventLogPtr *xref = msg->add_cross_ref();
        xref->set_tid(microEventLog->getOsThreadId());
        xref->set_next_instruction(nextIns);
    }
    PIN_ReleaseLock(&thread_state_lock_);

    // Stamp the message with its event id, then serialize
    // and write to output stream.
    msg->set_id(event_id_);
    if(event_id_ == std::numeric_limits<UINT32>::max()) {
        LOG_ERROR("Hit event count limit! Need event_id > 2^32-1 Aieeee!!!");
        assert(0);
    }
    event_id_++;
    WriteEventUnlocked(msg);

    PIN_ReleaseLock(&output_lock_);    
}

void
ZTracer::WriteEventUnlocked(const ztrace::Event *msg)
{
    UINT32 size = htonl( msg->ByteSize() ); 
    output_stream_.write((char *) &size, sizeof(size));
    msg->SerializeToOstream(&output_stream_);
}

void
ZTracer::Instrument()
{
    TRACE_AddInstrumentFunction(TraceMicroEvent, 0); 
    TRACE_AddInstrumentFunction(TraceDisassembly, this);
    IMG_AddInstrumentFunction(ImgTraceModuleLoad, this);
    IMG_AddUnloadFunction(ImgTraceModuleUnload, this);
    IMG_AddInstrumentFunction(ImgTraceMalloc, this);
    // INS_AddInstrumentFunction(InsTraceRegisters, );

#ifndef _WIN32
    //PIN_AddForkFunction(FPOINT_AFTER_IN_CHILD, FollowChild, 0);
    //PIN_AddForkFunction(FPOINT_AFTER_IN_PARENT, FollowParent, 0);
#endif
    PIN_AddFollowChildProcessFunction(PIN_Trampoline::FollowChildProcess, this);
    PIN_AddContextChangeFunction(LogContextChange, 0);
    PIN_AddApplicationStartFunction(PIN_Trampoline::ApplicationStart, this);
    PIN_AddFiniFunction(PIN_Trampoline::Finish, this);
    PIN_AddThreadStartFunction(PIN_Trampoline::ThreadStart, this);
    PIN_AddThreadFiniFunction(PIN_Trampoline::ThreadFinish, this);
    // TODO: CHECK, REORFG
}

VOID
ZTracer::OnApplicationStart()
{
    LOG_INFO("APPLICATION START PID: %d", PIN_GetPid());
}

VOID
ZTracer::OnFinish(INT32 code)
{
    INT pid = PIN_GetPid();
    LOG_WARN("END PID: %d PIN MEMORY: %d", pid, PIN_MemoryAllocatedForPin());

    // DEBUGGING!
    LOG_WARN("Dumping writable memory...");
    DumpAllWritableMemory();
    LOG_WARN("Done!");

    CloseLog();
}


BOOL
ZTracer::OnFollowChildProcess(CHILD_PROCESS childProcess)
{
    // TODO: log to runlog
    
    INT appArgc;
    CHAR const * const * appArgv;
    std::string cmdline;

    CHILD_PROCESS_GetCommandLine(childProcess, &appArgc, &appArgv);
    assert(appArgc > 0);
    for(int i=0, last=appArgc-1; i<appArgc; i++) {
        cmdline.append(appArgv[i]);
        if(i != last) {
            cmdline += " ";
        }
    }
    LOG_WARN("PID %d starting child with args: \"%s\"", PIN_GetPid(), cmdline.c_str());

    // Unconditionally Inject, for now...
    return TRUE; 
}

VOID
ZTracer::OnThreadStart(THREADID threadIndex, CONTEXT *ctx, INT32 flags)
{
    LOG_INFO("Thread id %d started", threadIndex);

    // Create a transaction log for the thread, set it as thread-local data
    PIN_GetLock(&thread_state_lock_, PIN_GetTid()); // XXX
    MicroEventLog *microEventLog = new MicroEventLog(this);
    ThreadData::SetMicroEventLog(microEventLog, threadIndex);
    live_threads_.insert(threadIndex);
    PIN_ReleaseLock(&thread_state_lock_); // XXX

    // Record thread start
    ztrace::Event ev = MakeEvent();
    ztrace::ThreadStart *thread_start = ev.mutable_thread_start();
    thread_start->set_flags(flags);
    WriteEvent(&ev);
}

VOID
ZTracer::OnThreadFinish(THREADID threadIndex, const CONTEXT *ctx, INT32 flags)
{
    LOG_INFO("Thread id %d finished", threadIndex);

    // Flush thread's micro event log
    MicroEventLog *microEventLog = ThreadData::get_micro_event_log(threadIndex);
    microEventLog->Flush();

    // Destroy thread's micro event log
    PIN_GetLock(&thread_state_lock_, PIN_GetTid());
    delete microEventLog;    
    live_threads_.erase(threadIndex); // TODO: lock here?
    ThreadData::SetMicroEventLog(0, threadIndex);
    PIN_ReleaseLock(&thread_state_lock_);

    // Record thread completion
    ztrace::Event ev = MakeEvent();
    ztrace::ThreadEnd *thread_end = ev.mutable_thread_end();
    thread_end->set_flags(flags);
    WriteEvent(&ev);
}


static void
set_timestamp_to_now(ztrace::Timestamp *ts)
{
#ifndef _WIN32
    int ret;
    
    struct timeval tv;
    ret = gettimeofday(&tv, NULL);
    assert(ret == 0);

    ts->set_sec(tv.tv_sec);
    ts->set_usec(tv.tv_usec);
#else
#error "Win32 support not complete."
#endif
}

static ztrace::Platform
get_platform()
{
#ifdef __APPLE__
  return ztrace::PLATFORM_OSX;
#elif __linux__
  return ztrace::PLATFORM_LINUX;
#elif WIN32
  return ztrace::PLATFORM_WINDOWS;
#else
#error "Unknown platform!"
#endif
}


void
ZTracer::LogTraceStart()
{
    ztrace::Event ev = MakeEvent();
    ztrace::TraceStart *ev_start = ev.mutable_trace_start();
    ztrace::Timestamp *ts = ev_start->mutable_timestamp();
    set_timestamp_to_now(ts);
    // Yeah, yeah, I know...
    ev_start->set_arch(sizeof(void *) == 8 ? ztrace::ARCH_X86_64 : ztrace::ARCH_X86);
    ev_start->set_platform(get_platform());
    ev_start->set_software("ZTrace");
    //ev_start->set_target();
    //ev_start->set_cmdline();
    //ev_start->set_os_unmae();
    WriteEvent(&ev);
}

void
ZTracer::LogTraceEnd()
{
    ztrace::Event ev = MakeEvent();
    ztrace::TraceEnd *ev_end = ev.mutable_trace_end();
    ztrace::Timestamp *ts = ev_end->mutable_timestamp();
    set_timestamp_to_now(ts);
    WriteEvent(&ev);
}

void 
ZTracer::DumpMemoryRegion(ADDRINT addr, ADDRINT endaddr, std::string perms)
{
  LOG_WARN("dumping!!!");

  ztrace::Event ev = MakeEvent();
  ztrace::MemoryDump *mem_dump = ev.mutable_memory_dump();
  ztrace::MemoryRegion *mem_region = mem_dump->mutable_region();

  mem_region->set_addr(addr);
  mem_region->set_endaddr(addr);
  mem_region->set_perms(perms);

  assert(endaddr > addr);
  size_t size = endaddr - addr;

  std::vector<unsigned char> buf(size);
  size_t ret = PIN_SafeCopy(&buf[0], (void *)addr, size);
  if(ret != size) {
    LOG_WARN("DumpMemoryRegion: expected %d bytes, got %d", size, ret);
  }
  mem_dump->set_data(&buf[0], size);
  
  WriteEvent(&ev);
}

void 
ZTracer::DumpAllWritableMemory()
{
  std::vector<MemoryRegion> regions;
  bool ret = read_memory_regions(&regions);
  if(!ret) {
    LOG_WARN("DumpAllWritableMemory: failed to get memory map :(");
    return;
  }
  std::vector<MemoryRegion>::iterator it;
  for(it = regions.begin(); it != regions.end(); ++it) {
    LOG_WARN("Region: %p -> %p (%s)", it->addr, it->endaddr, it->perms.c_str());
    if(it->perms.find('w') != std::string::npos) {
      DumpMemoryRegion(it->addr, it->endaddr, it->perms);
    }
  }
}
