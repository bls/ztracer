#!/usr/bin/python 

import ZTrace, sys, collections

# TODO: handle longjmp 
# TODO: handle _not_ calling into libc or similar (like Maid)

class Activation(object):
    """Activation record / stack frame."""
    def __init__(self, op):
        self.target = op.target
        self.sp = op.sp
        self.is_direct_call = op.is_direct_call

def main():
    addr2sym = {}
    callrets = []
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        msg_type = trace.get_event_type(msg)

        # Build symbol table
        if msg_type == 'mod_symbols':
            for sym in msg.mod_symbols.sym:
                addr2sym[sym.address] = "%s # %s" % (sym.undecorated_name, msg.mod_symbols.module_name)

        # Store CALL / RET
        if msg_type == 'micro_event_log':
            for op in trace.decode_micro_events(msg):
                if op.type == ZTrace.OP_CALL or op.type == ZTrace.OP_RET:
                    callrets.append(op)
   
    indent = 0
    activations = []
    for op in callrets:
        if op.type == ZTrace.OP_CALL:
            symbol = '?'
            if op.target in addr2sym:
                symbol = addr2sym[op.target]
            print "%s CALL %x (Direct? %d) [%s]" % (".." * len(activations), op.target, op.is_direct_call, symbol)
            activations.append(Activation(op))
        if op.type == ZTrace.OP_RET:
            if not activations:
                print "Stack underflow from RET!"
            else:
                expected_sp = activations[-1].sp
                if op.sp > expected_sp:
                    print "Unexpected SP: %x < %x ! (longjmp?)" % (expected_sp, op.sp)
                print "%s RET" % (".." * len(activations))
                if len(activations) > 0:
                    activations.pop()
                else:
                    print "Stack underflow during RET!"
    

if __name__ == '__main__':
    main()
    
