#ifndef _HELPERS_H
#define _HELPERS_H

#include "pin.H"

REG GetScratchReg(UINT32 index);

void LogContextChange(THREADID threadIndex, CONTEXT_CHANGE_REASON reason, 
                      const CONTEXT *ctxtFrom, CONTEXT *ctxtTo, INT32 sig, VOID *v);

BOOL LogFollowChild(CHILD_PROCESS childProcess, VOID *v);

#endif 
