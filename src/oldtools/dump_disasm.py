#!/usr/bin/python 

import ZTrace, sys, collections, binascii

def main():
    dups = collections.defaultdict(int)
    unique_ins = 0
    ins_text = {}
    ins_bytes = {}
    trace_addrs = set()
    bbl_addrs = set()

    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        trace_start = None # record PC of trace start
        if msg.HasField("trace_disasm"):
            for bbl in msg.trace_disasm.bbl_disasm:
                bbl_start = None # record PC of bbl start
                for ins in bbl.ins_disasm:
                    # Handle TRACE or BBL start
                    if not trace_start:
                        trace_start = ins.pc
                        trace_addrs.add(ins.pc)
                    if not bbl_start:
                        bbl_start = ins.pc
                        bbl_addrs.add(ins.pc)
                    # Handle duplicate disasm at PC
                    if ins.pc in ins_text:
                        dups[ins.pc] += 1
                        # Did the instruction change?
                        if ins_text[ins.pc] != ins.text:
                            print "!!! Changed disasm @ %p" % (ins.pc)
                            print "!!! Was: %s" % (ins_text[ins.pc])
                            print "!!! Now: %s" % (ins.text)
                    else:
                        unique_ins += 1
                    # Disassembly at PC as per trace
                    ins_text[ins.pc] = ins.text
                    ins_bytes[ins.pc] = ins.raw_bytes

    # TODO: show modules?
    # TODO: identify code running OUTSIDE modules?
    # TODO: show call targets?
    # TODO: option for BBL / trace boundaries?
    # TODO: verbose dups?

    next_pc = 0
    for pc in sorted(ins_text.keys()):
        if pc != next_pc:
            print "..."
        #if pc in trace_addrs: 
        #    print "TRACE:   %08x" % (pc)
        #if pc in bbl_addrs:
        #    print "    BBL: %08x" % (pc)
        print "         %08x:  %-24s  %s" % (pc, binascii.hexlify(ins_bytes[pc]), ins_text[pc])
        next_pc = pc + len(ins_bytes[pc])
    
    print "# unique instructions: %d" % (unique_ins)
    print "# duplicate instructions: %d" % sum(dups.values())
        
if __name__ == '__main__':
    main()
    
