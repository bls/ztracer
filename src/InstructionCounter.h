#ifndef _INSTRUCTION_COUNTER_H
#define _INSTRUCTION_COUNTER_H

#include "pin.H"
#include <limits>
#include <cstring>

// 64-bit counter; designed for following:
// * Needs to work on X86 and X86_64.
// * One writer thread, increments VERY frequently.
// * One reader thread, reads quite infrequently.
// * OK for the reader to see a slightly out-of-date (lower)
//   value, but torn reads are not OK.


class InstructionCounter 
{
public:
    InstructionCounter() {
        memset((void *) &counter_, 0, sizeof(UINT32) * 2);
        PIN_InitLock(&lock_);
    }

    inline void inc() {
        if(counter_[0] < std::numeric_limits<UINT32>::max()) {
            // Fast path relies on atomic 32-bit reads. Since we
            // are not carrying, no tearing is possible.
            counter_[0]++;
        } else {
            PIN_GetLock(&lock_, PIN_GetTid());
            counter_[0]++;
            counter_[1]++;
            PIN_ReleaseLock(&lock_);
        }
    }

    inline UINT64 read() {
        UINT64 result;
        PIN_GetLock(&lock_, PIN_GetTid());
        result = (UINT64) counter_[1] << 32 | counter_[0];
        PIN_ReleaseLock(&lock_);
        return result;
    }

    
private:
    UINT32 volatile counter_[2];
    PIN_LOCK lock_;    
};

#endif 
