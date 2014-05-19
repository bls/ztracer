#include "TraceRegisters.h"
#include "MicroEventLog.h"
#include "Log.h"


LOG_INIT("TraceRegisters");


#define LOG_XMM  1


BOOL IsRegGeneralPurpose(REG reg)
{
    return REG_is_gr64(reg) || REG_is_gr32(reg) || REG_is_gr16(reg) || REG_is_gr8(reg);
}


BOOL IsRegFlags(REG reg)
{
#ifdef TARGET_IA32
    return reg == REG_FLAGS || reg == REG_EFLAGS;
#else
    return reg == REG_FLAGS || reg == REG_EFLAGS || reg == REG_RFLAGS;
#endif
}

BOOL IsRegEIP(REG reg)
{
#ifdef TARGET_IA32
    return reg == REG_IP;
#else
    return reg == REG_IP || reg == LEVEL_BASE::REG_RIP;
#endif
}

VOID AddLogRegisterWrite(INS ins, IPOINT where, REG reg);
VOID AddLogRegisterWriteXMM(INS ins, IPOINT where, REG xmm_reg, UINT32 size);


VOID InsTraceRegisters(INS ins, VOID *v)
{
    // http://tech.groups.yahoo.com/group/pinheads/message/1362
    // INS_OperandIsImplicit(ins, i) - logging implicit operands too
    // TODO: may need special handling for rip register. Probably better
    // not to log this explicitly.
    
    for(UINT32 opIdx = 0; opIdx < INS_OperandCount(ins); opIdx++)
    {
        if(INS_OperandIsReg(ins, opIdx) && INS_OperandWritten(ins, opIdx))
        {
            std::string disas; // XXX
            REG reg = INS_OperandReg(ins, opIdx);
            std::string regname = REG_StringShort(reg);

            if(reg == REG_INVALID()) // Unexpected
            {
                LOG_ERROR("InsRegisterTrace: bad register in: %s # %s",
                          disas.c_str(), regname.c_str());
            }
            else if(REG_is_any_x87(reg))  // FP regs not handled
            {
                //LOG_DEBUG("InsRegisterTrace: not capturing x87 register write: %s # %s",
                //          disas.c_str(), regname.c_str());
            }
            else if(IsRegEIP(reg))
            {
                ; // Don't bother logging EIP changes, just follow it..
            }
            else if(IsRegGeneralPurpose(reg) || IsRegFlags(reg)) // GP regs are logged
            {

                if (INS_HasFallThrough(ins))
                    AddLogRegisterWrite(ins, IPOINT_AFTER, reg);
                if (INS_IsBranchOrCall(ins))
                    AddLogRegisterWrite(ins, IPOINT_TAKEN_BRANCH, reg);
            }
            else if(REG_is_xmm(reg) || REG_is_mm(reg)) // XMM/MMX regs are logged
            {
#ifdef LOG_XMM
                UINT32 width = INS_OperandWidth(ins, opIdx);
                UINT32 size = width / 8;
                assert(width % 8 == 0);
                
                if (INS_HasFallThrough(ins))
                    AddLogRegisterWriteXMM(ins, IPOINT_AFTER, reg, size);
                if (INS_IsBranchOrCall(ins))
                    AddLogRegisterWriteXMM(ins, IPOINT_TAKEN_BRANCH, reg, size);
#else
                // Nop. 
#endif
            }
            else
            {
                LOG_ERROR("InsRegisterTrace: unknown register type in: %s # %s",
                          disas.c_str(), regname.c_str());
            }
        }
    } 
}


VOID LogRegisterWrite(THREADID tid, ADDRINT pc, UINT32 reg, ADDRINT value)
{
    //Log[tid].logRegisterWrite(pc, reg, value);
}


VOID AddLogRegisterWrite(INS ins, IPOINT where, REG reg)
{
    INS_InsertPredicatedCall(ins, where, AFUNPTR(LogRegisterWrite),
                             IARG_THREAD_ID, IARG_INST_PTR,
                             IARG_UINT32, reg,
                             IARG_REG_VALUE, reg,
                             IARG_END);
}


VOID LogRegisterWriteXMM(THREADID tid, ADDRINT pc, UINT32 xmm_reg, PIN_REGISTER *xmm_reg_ref, UINT32 size)
{
    // Log[tid].logRegisterWriteXMM(pc, xmm_reg, xmm_reg_ref->byte, size);
}


VOID AddLogRegisterWriteXMM(INS ins, IPOINT where, REG xmm_reg, UINT32 size)
{
    INS_InsertPredicatedCall(ins, where, AFUNPTR(LogRegisterWrite),
                             IARG_THREAD_ID, IARG_INST_PTR,
                             IARG_UINT32, xmm_reg,
                             IARG_REG_CONST_REFERENCE, xmm_reg,
                             IARG_UINT32, size,
                             IARG_END);
}

