#!/usr/bin/python

import ZTrace_pb2, struct, sys

msg = ZTrace_pb2.Event()
with open(sys.argv[1]) as fp:
    while fp:
        # Read packet length; length is encoded
        # in network byte order
        t = fp.read(4)
        if not t:
             break # Termination
        size = struct.unpack('!I', t)[0]

        # Read packet and parse into protobuf
        data = fp.read(size)
        msg.ParseFromString(data)

        # OK can process Event message ...
        print str(msg)

