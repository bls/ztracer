#!/usr/bin/python 

import ZTrace, sys

# TODO: want some concept of *when* allocated
# TODO: need some contect of *when* freed :)
# TODO: the above wants stack traces, really
# TODO: pointer analysis!
# TODO: consider use of interval trees or Banyan
# TODO: dump readers / writers?
# TODO: need to track SP to detect stack accesses; e.g. get
#       lowest and highest stack addrs... this comes under
#       register write logging; 

def hexdump(src, length=16):
    if type(src) == str:
        src = bytearray(src)
    FILTER = ''.join([(len(repr(chr(x))) == 3) and chr(x) or '.' for x in range(256)])
    lines = []
    for c in xrange(0, len(src), length):
        bytes = src[c:c+length]
        # hex = ' '.join(["%02x" % ord(x) for x in chars])
        hex = ' '.join(["%02x" % x for x in bytes])
        printable = ''.join(["%s" % ((x <= 127 and FILTER[x]) or '.') for x in bytes])
        lines.append("%04x  %-*s  %s\n" % (c, length*3, hex, printable))
    return ''.join(lines)

class HeapChunk(object):
    def __init__(self, p, n):
        self.p = p
        self.n = n
        self.data = bytearray(n)
        self.readers = set()
        self.writers = set()

    def contains_addr(self, p):
        return self.p <= p < (self.p + self.n)

    def write(self, ea, data):
        #print "ea = %08x" % ea
        ea -= self.p
        #print "p  = %08x" % self.p
        #print "chunk len = %d" % self.n
        #print "contents len = %d" % len(self.data)
        #print "offset = %d" % ea
        #print "write len = %d" % len(data)
        #print "DATA"
        #print hexdump(data)
        if ea + len(data) > self.n:
            overflow_bytes = ea + len(data) - self.n
            print "Overflow write to chunk @%0x8 - %d bytes" % (self.p, overflow_bytes)
            data = data[0:-overflow_bytes]
        self.data[ea:ea+len(data)] = data
        #print len(self.data)
        #print self.n
        assert len(self.data) == self.n

chunks = {}

def find_chunk(p):
    for chunk in chunks.values():
        if chunk.contains_addr(p):
            return chunk
    return None

def main():
    trace = ZTrace.TraceFile(sys.argv[1])
    for msg in trace.events():
        if msg.HasField("malloc"):
            (p, n) = (msg.malloc.ret, msg.malloc.n)
            chunks[p] = HeapChunk(p, n)
            print "malloc'd chunk @%08x (len %d)" % (p, n)
        if msg.HasField("free"):
            p = msg.free.p
            if p != 0:
                if p not in chunks:
                    print "WARNING: free() of unexpected address!"
                else:
                    chunk = chunks[p]
                    del chunks[p]
                    print "--"
                    print "free'd chunk @%08x" % (chunk.p)
                    print hexdump(chunk.data)
                    print "WRITERS: " + ' '.join(["%08x" % x for x in chunk.writers])
        if msg.HasField("micro_event_log"):
            for op in trace.decode_micro_events(msg):
                if op.type == ZTrace.OP_MEMREAD:
                    pass
                if op.type == ZTrace.OP_MEMWRITE:
                    chunk = find_chunk(op.ea)
                    if chunk is not None:
                        # print "heap write to chunk %08x @ %08x" % (chunk.p, op.ea)
                        chunk.write(op.ea, op.data)
                        chunk.writers.add(op.pc)
                    else:
                        pass
                        # print "non heap write %08x" % (op.ea)

    print "======================================="
    print "PROGRAM ENDED!"
    print "======================================="    

    # Dump any leaked chunks
    for chunk in chunks.values():
        print "--"        
        print "leaked chunk @%08x" % (chunk.p)
        print hexdump(chunk.data)
        print "WRITERS: " + ' '.join(["%08x" % x for x in chunk.writers])        
        
if __name__ == '__main__':
    main()
    
