#ifndef _MICRO_EVENT_LOG_H
#define _MICRO_EVENT_LOG_H

#include "pin.H"
#include <ostream>
#include "ZTrace.pb.h"
#include "ByteSink.h"
#include "InstructionCounter.h"

#define MAX_LOG_SIZE       (64 * 1024)
// Next value is fairly arbitrary; if micro events for a
// single instruction could possibly take more space than
// this, bump the value.
#define MIN_LOG_FREE_SPACE (64 * 8)

#define OP_NEWPC    0x01
#define OP_MEMREAD  0x02
#define OP_MEMWRITE 0x04
#define OP_REGWRITE 0x08
#define OP_XMMWRITE 0x10
#define OP_CALL     0x20
#define OP_RET      0x40

#define INLINE inline

class ZTracer;

// Note: this only ever gets read or written by the owning
// thread.
class MicroEventLog
{
public:
    MicroEventLog(ZTracer *ztracer);

    INLINE UINT64
    getNextInstructionIndex()
    {
        return insCounter_.read();
    }

    INLINE OS_THREAD_ID
    getOsThreadId()
    {
        return osThreadId_;
    }
    
    INLINE void
    logPC(ADDRINT pc)
    {
        // We only flush on instruction boundaries; this
        // means analysis tools don't have to track micro-ops
        // across multiple event records.
        CheckFlush();

        // When we see a new PC, we know that all ops from 
        // previous instruction on this thread are written. 
        insCounter_.inc();
        
        log_.put<UINT8>(OP_NEWPC);
        log_.put(pc);
    }

    INLINE void
    logMemoryWrite(ADDRINT ea, UINT8 *data, UINT32 size)
    {
        log_.put<UINT8>(OP_MEMWRITE);
        log_.put(ea);
        log_.put(size);
        log_.put(data, size);
    }

    INLINE void
    logMemoryRead(ADDRINT ea, UINT32 size)
    {
        log_.put<UINT8>(OP_MEMREAD);
        log_.put(ea);
        log_.put(size);
    }

    INLINE void
    logRegisterWrite(UINT32 reg, ADDRINT value)
    {
        log_.put<UINT8>(OP_REGWRITE);
        log_.put(reg);
        log_.put(value);
    }

    INLINE void
    logCall(ADDRINT target, ADDRINT sp, BOOL is_direct)
    {
        log_.put<UINT8>(OP_CALL);
        log_.put(target);
        log_.put(sp);
        log_.put(is_direct);
    }

    INLINE void
    logRet(ADDRINT sp)
    {
        log_.put<UINT8>(OP_RET);
        log_.put(sp);
    }

    INLINE void
    logRegisterWriteXMM(UINT32 xmm_reg, UINT8 *value, UINT32 size)
    {
        log_.put<UINT8>(OP_XMMWRITE);
        log_.put(xmm_reg);
        log_.put(size);
        log_.put(value, size);
    }

    void Flush();
    
private:
    void
    CheckFlush()
    {
        if(log_.space() < MIN_LOG_FREE_SPACE) {
            Flush();
        }
    }

private:
    UINT64 insStart_; // Start ins count for current micro event msg
    InstructionCounter insCounter_; // Current ins counter for thread
    ZTracer *ztracer_; // Backref to ZTracer for event output
    ByteSink log_; // Byte buffer to hold micro events
    OS_THREAD_ID osThreadId_; // Operating system thread ID of owner
};

#endif 
