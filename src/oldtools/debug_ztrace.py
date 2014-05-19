#!/usr/bin/python 

import ZTrace, sys, collections, traceback

def main():
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        msg_type = trace.get_event_type(msg)
        if msg_type == 'micro_event_log':
            buf = ''
            try:
                for micro_event in trace.decode_micro_events(msg):
                    buf += str(micro_event) + "\n"
            except:
                print "Exception parsing micro-events:"
                traceback.print_exc()
                print buf
                
if __name__ == '__main__':
    main()
    
