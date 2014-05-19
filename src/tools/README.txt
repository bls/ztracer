
Want to create something sequential from this loosely consistent trace file.
What should database schema look like?

OK, so each and every event gets an ID. Now, how do we store the damn things? We basically 
want to be able to query the state of the system at various points in time, ideally by just
quickly rebuilding them.

OK, what if I do a first pass, unpacking all the MicroEvents into a table so that we can 
cross-reference them?  That's a reasonable idea, right?

Full:

Created database schema
Done!

real	2m21.544s
user	2m19.530s
sys	0m1.060s

Completely remove micro op logging:

bls@zxc:/opt/pin/source/xpintools/ztracer/dbtools$ time ./make_trace_db.py ../run.trace.10122  
Created database schema
Done!

real	0m5.218s
user	0m4.100s
sys	0m0.180s

Just run through the micro ops (op decoding):

bls@zxc:/opt/pin/source/xpintools/ztracer/dbtools$ time ./make_trace_db.py ../run.trace.10122  
Created database schema
Done!

real	1m14.576s
user	1m13.080s
sys	0m0.570s

Disabled self check code, shaved off 13 secs:

bls@zxc:/opt/pin/source/xpintools/ztracer/dbtools$ time ./make_trace_db.py ../run.trace.10122  
Created database schema
Done!

real	1m4.216s
user	1m2.650s
sys	0m0.610s

With pickle (rather than op serialization!) - OUCH!

bls@zxc:/opt/pin/source/xpintools/ztracer/dbtools$ time ./make_trace_db.py ../run.trace.10122  
Created database schema
Done!

real	4m42.651s
user	4m32.140s
sys	0m8.490s

OK, that seems kind of slow... Like, really slow...
