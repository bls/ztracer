#include "TraceDisassembly.h"
#include "ZTracer.h"
#include "ZTrace.pb.h"
#include "Log.h"
#include "assert.h"

LOG_INIT("TraceDisassembly");

#define MAX_INS_BYTES 256

VOID TraceDisassembly(TRACE trace, VOID *v)
{
    ZTracer *ztracer = static_cast<ZTracer *>(v);
    CHAR ins_data[MAX_INS_BYTES];

    ztrace::Event ev = ztracer->MakeEvent();
    ztrace::TraceDisassembly *trace_disasm = ev.mutable_trace_disasm();
    
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl))
    {
        ztrace::BBLDisassembly *bbl_disasm = trace_disasm->add_bbl_disasm();
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins))
        {
            // Instruction disasm
            ADDRINT eip = INS_Address(ins);
            std::string disas = INS_Disassemble(ins);    
            ztrace::InsDisassembly *ins_disasm = bbl_disasm->add_ins_disasm();
            ins_disasm->set_pc(eip);
            ins_disasm->set_text(disas);
            // Copy the raw instruction bytes from process
            USIZE ins_size = INS_Size(ins);
            assert(ins_size < MAX_INS_BYTES);
            size_t ret = PIN_SafeCopy(ins_data, (const void *) eip, ins_size);
            if(ret != ins_size) {
                LOG_ERROR("%p: PIN_SafeCopy expected: %d got: %d",
                          (void *) eip, ins_size, ret);
                
            } else {
                ins_disasm->set_raw_bytes(ins_data, ins_size);
            }
        }
    }
     
    ztracer->WriteEvent(&ev);
}
