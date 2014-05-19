#include "TraceMicroEvent.h"
#include "ThreadData.h"
#include "MicroEventLog.h"
#include "Log.h"

LOG_INIT("TraceMicroEvent");

// MAX_BYTES_PER_PIN_REG is 32 (256 bits), however this isn't
// exported anywhere. Note: as of 2013, intel are planning to
// add 512 bit "super avx" registers. Not there yet tho :)
#define MAX_WRITE_SIZE 32

static VOID *WriteEa[PIN_MAX_THREADS];

static VOID AddLogPC(INS ins);
static VOID AddLogMemoryRead(INS ins, UINT32 memOp, UINT32 size);
static VOID AddCaptureWriteEa(INS ins, UINT32 memOp);
static VOID AddLogMemoryWrite(INS ins, IPOINT where, UINT32 size);
static VOID AddLogCall(INS ins, TRACE trace);
static VOID AddLogRet(INS ins);


VOID
TraceMicroEvent(TRACE trace, VOID *v)
{
    for( BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl) )
    {
        for( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) )
        {
            UINT32 memoryOperands = INS_MemoryOperandCount(ins);
            
            // Log every PC
            AddLogPC(ins); 

            // Log memory reads and writes
            for (UINT32 memOp = 0; memOp < memoryOperands; memOp++)
            {
                UINT32 refSize = INS_MemoryOperandSize(ins, memOp);
                if (INS_MemoryOperandIsRead(ins, memOp))
                {
                    AddLogMemoryRead(ins, memOp, refSize);
                }
                if (INS_MemoryOperandIsWritten(ins, memOp))
                {
                    AddCaptureWriteEa(ins, memOp);
            
                    if (INS_HasFallThrough(ins))
                    {
                        AddLogMemoryWrite(ins, IPOINT_AFTER, refSize);
                    }
                    if (INS_IsBranchOrCall(ins))
                    {
                        AddLogMemoryWrite(ins, IPOINT_TAKEN_BRANCH, refSize);                
                    }
                }
            }
        } // For each INS

        INS tail = BBL_InsTail(bbl);

        // All calls and returns
        if( INS_IsSyscall(tail) ) {
            // INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
            //                          (AFUNPTR)A_ProcessSyscall,
            //                          IARG_INST_PTR,
            //                          IARG_SYSCALL_NUMBER,
            //                          IARG_REG_VALUE, REG_STACK_PTR,
            //                          IARG_SYSCALL_ARG0,
            //                          IARG_END);
            ;
        } else {
            if( INS_IsCall(tail) ) {
                AddLogCall(tail, trace);
            }
            if( INS_IsRet(tail) ) {
                AddLogRet(tail);
            }
        } 
        
    } // For each BBL
}


// ---------------------------------------------------
// Log changes to EIP / program counter (PC)
// ---------------------------------------------------

static VOID
LogPC(THREADID tid, ADDRINT pc)
{
    ThreadData::get_micro_event_log(tid)->logPC(pc);
}


static VOID
AddLogPC(INS ins)
{
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(LogPC),
                   IARG_CALL_ORDER, CALL_ORDER_FIRST,
                   IARG_THREAD_ID,
                   IARG_INST_PTR,
                   IARG_END);
}

// ---------------------------------------------------
// Log memory reads & writes
// ---------------------------------------------------

static VOID
CaptureWriteEa(THREADID tid, VOID *ea)
{
    WriteEa[tid] = ea;
}


static VOID
AddCaptureWriteEa(INS ins, UINT32 memOp)
{
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CaptureWriteEa),
                   IARG_THREAD_ID,
                   IARG_MEMORYOP_EA, memOp,
                   IARG_END);
}


static VOID
LogMemoryWrite(THREADID tid, ADDRINT pc, UINT32 size)
{
    const ADDRINT& ea = (ADDRINT) WriteEa[tid];
    UINT8 data[MAX_WRITE_SIZE];

    assert(size <= MAX_WRITE_SIZE);
    size_t ret = PIN_SafeCopy(data, (const void *) ea, size);
    if(ret != size) {
        LOG_ERROR("LogMemoryWrite: %p: PIN_SafeCopy expected: %d got: %d",
                  (void *) pc, size, ret);
    }
    ThreadData::get_micro_event_log(tid)->logMemoryWrite(ea, data, ret);
}


static VOID
AddLogMemoryWrite(INS ins, IPOINT where, UINT32 size)
{
    INS_InsertPredicatedCall(ins, where, AFUNPTR(LogMemoryWrite),
                             IARG_THREAD_ID, IARG_INST_PTR,
                             IARG_UINT32, size,
                             IARG_END);
}


static VOID
LogMemoryRead(THREADID tid, ADDRINT ea, UINT32 size)
{
    ThreadData::get_micro_event_log(tid)->logMemoryRead(ea, size);
}


static VOID
AddLogMemoryRead(INS ins, UINT32 memOp, UINT32 size)
{
    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(LogMemoryRead),
                             IARG_THREAD_ID, 
                             IARG_MEMORYOP_EA, memOp,
                             IARG_UINT32, size,
                             IARG_END);
}

// ---------------------------------------------------
// Log call / return for stack tracing
// Most of this code ripped from Maid.
// ---------------------------------------------------

static BOOL
IsPLT(TRACE trace)
{
    RTN rtn = TRACE_Rtn(trace);

    // All .plt thunks have a valid RTN
    if (!RTN_Valid(rtn))
        return FALSE;

    if (".plt" == SEC_Name(RTN_Sec(rtn)))
        return TRUE;
    return FALSE;
}

static VOID
LogCall(THREADID tid, ADDRINT target, ADDRINT sp, BOOL is_direct)
{
    ThreadData::get_micro_event_log(tid)->logCall(target, sp, is_direct);
}

// TODO: passing trace below is a bit of a hack :-/
static VOID 
AddLogCall(INS tail, TRACE trace)  
{
    // TODO: assert call / branch!
    if( INS_IsDirectBranchOrCall(tail) ) {
        ADDRINT target = INS_DirectBranchOrCallTargetAddress(tail);
        INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
                                 (AFUNPTR)LogCall,
                                 IARG_THREAD_ID,                                 
                                 IARG_ADDRINT, target,
                                 IARG_REG_VALUE, REG_STACK_PTR,
                                 IARG_BOOL, false,
                                 IARG_END);
    } else if( !IsPLT(trace) ) {
        INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
                                 (AFUNPTR)LogCall,
                                 IARG_THREAD_ID,
                                 IARG_BRANCH_TARGET_ADDR,
                                 IARG_REG_VALUE, REG_STACK_PTR,
                                 IARG_BOOL, false,
                                 IARG_END);
    }
}

static VOID
LogRet(THREADID tid, ADDRINT sp)
{
    ThreadData::get_micro_event_log(tid)->logRet(sp);    
}

static VOID 
AddLogRet(INS tail)
{
    // TODO: assert ret!
    INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
                             (AFUNPTR)LogRet,
                             IARG_THREAD_ID,
                             IARG_REG_VALUE, REG_STACK_PTR,
                             IARG_END);
}


