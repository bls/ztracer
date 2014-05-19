#!/usr/bin/python 

import ZTrace, sys, collections, traceback

cnt = collections.defaultdict(int)
msgbytes = collections.defaultdict(int)
opcnts = collections.defaultdict(int)


def main():
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        msg_type = trace.get_event_type(msg)
        cnt[msg_type] += 1
        msgbytes[msg_type] += msg.ByteSize()

        if msg_type == 'micro_event_log':
            try:
                for micro_event in trace.decode_micro_events(msg):
                    op_name = micro_event.op_name()
                    opcnts[op_name] += 1
            except:
                traceback.print_exc()
                
    print "=== Event stats ==="
    for k in sorted(cnt.keys()):
        print "%s => %d event messages, %d bytes total" % (k, cnt[k], msgbytes[k])

    print

    print "=== Micro Event stats ==="
    for k in sorted(opcnts.keys()):
        print "%s => %d logs" % (k, opcnts[k])

        
if __name__ == '__main__':
    main()
    
