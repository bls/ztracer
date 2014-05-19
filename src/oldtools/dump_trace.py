#!/usr/bin/python 

import ZTrace, sys

def main():
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        if msg.HasField("micro_event_log"):
            for micro_event in trace.decode_micro_events(msg):
                print "#" + str(micro_event)

            msg.micro_event_log.data = "[ ... ]"

        if msg.HasField("mod_load"):
            msg.mod_load.contents = "[ ... ]"
 
        print str(msg)

        
if __name__ == '__main__':
    main()
    
