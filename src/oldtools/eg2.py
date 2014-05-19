#!/usr/bin/python 

import ZTrace, sys

def main():
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        msg_type = trace.get_event_type(msg)
        print msg_type
        
if __name__ == '__main__':
    main()
    
