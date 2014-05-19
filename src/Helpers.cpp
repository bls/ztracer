#include <signal.h>
#include "Helpers.h"
#include "Log.h"

LOG_INIT("Helpers");

static const char *
signal_name_str(int sig) 
{
    switch (sig) {
    case SIGUSR1: return "SIGUSR1";
    case SIGUSR2: return "SIGUSR2";
    case SIGSEGV: return "SIGSEGV";
    case SIGCONT: return "SIGCONT";
    case SIGINT:  return "SIGINT";
    default:
        return "?";
    }
}

static const char *
context_change_reason_str(CONTEXT_CHANGE_REASON reason)
{
    switch(reason) {
    case CONTEXT_CHANGE_REASON_SIGNAL: return "SIGNAL";
    case CONTEXT_CHANGE_REASON_SIGRETURN: return "SIGRETURN";
    case CONTEXT_CHANGE_REASON_FATALSIGNAL: return "FATALITY";
    case CONTEXT_CHANGE_REASON_APC: return "WINAPC";
    case CONTEXT_CHANGE_REASON_EXCEPTION: return "WINEXCEPT";
    case CONTEXT_CHANGE_REASON_CALLBACK: return "WINCALLBACK";
    default:
        return "?";
    }
}

void
LogContextChange(THREADID threadIndex, CONTEXT_CHANGE_REASON reason, 
                const CONTEXT *ctxtFrom, CONTEXT *ctxtTo, INT32 sig, VOID *v)
{
    const char *signame __attribute__ ((unused)) = signal_name_str(sig);
    const char *desc __attribute__ ((unused)) = context_change_reason_str(reason);
    ADDRINT fromIP __attribute__ ((unused)) = 0;
    if(ctxtTo) {
        fromIP = PIN_GetContextReg(ctxtFrom, REG_INST_PTR);
    }
    ADDRINT toIP __attribute__ ((unused)) = 0;
    if(ctxtTo) {
        toIP = PIN_GetContextReg(ctxtTo, REG_INST_PTR);
    }
    INT pid = PIN_GetPid();

    switch (reason)
    {
    case CONTEXT_CHANGE_REASON_FATALSIGNAL:
    case CONTEXT_CHANGE_REASON_SIGNAL:
    case CONTEXT_CHANGE_REASON_SIGRETURN:
    case CONTEXT_CHANGE_REASON_EXCEPTION:
        LOG_WARN("CTXT CHANGE: (%s) %d/%d (%p -> %p) sig %d (%s)",
                 desc, pid, threadIndex, (void *) fromIP, (void *) toIP, sig, signame);
        break;
    case CONTEXT_CHANGE_REASON_APC:
    case CONTEXT_CHANGE_REASON_CALLBACK:
        LOG_DEBUG("CTXT CHANGE: (%s) %d/%d (%p -> %p) sig %d (%s)",
                  desc, pid, threadIndex, (void *) fromIP, (void *)toIP, sig, signame);
        break;
    }
}

BOOL
LogFollowChild(CHILD_PROCESS childProcess, VOID *v)
{
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
    //LOG_WARN("PID %d starting child with args: \"%s\"", PIN_GetPid(), cmdline.c_str());
    LOG_WARN("PID %d starting child with args: \"%s\"", PIN_GetPid(), "WHATEVA");    

    return TRUE; // Inject.
}

REG
GetScratchReg(UINT32 index)
{
    static vector<REG> regs;
    while (index >= regs.size()) {
        REG reg = PIN_ClaimToolRegister();
        if (reg == REG_INVALID()) {
            LOG_ERROR("Ran out of tool registers!");
            PIN_ExitProcess(1);
        }
        regs.push_back(reg);
    }
    return regs[index];
}



