#!/usr/bin/python 

import ZTrace, sys, collections

def main():
    addr2sym = {}
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        msg_type = trace.get_event_type(msg)
        if msg_type == 'mod_symbols':
            for sym in msg.mod_symbols.sym:
                addr2sym[sym.address] = "%s # %s" % (sym.undecorated_name, msg.mod_symbols.module_name)
                
    for k in sorted(addr2sym.keys()):
        print "%0x => %s" % (k, addr2sym[k])

if __name__ == '__main__':
    main()
    
