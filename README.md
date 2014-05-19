ZTRACER
=======

Program tracer, based on PIN, Protobufs and Python.

## INSTALL ##

Installation instructions for Linux, OSX and Windows are available in the "install" directory.

## RUN ##

* You need to set PIN env vars before working with ztracer:

      cd path/to/ztracer
      source pin_env.sh

* To produce a trace, run:

      ./ZTracer.sh ls

* Output should be in "run.trace.$pid" - you can translate this into a more human-readable 
  format with "dump_trace.py":

      ./tools/dump_trace.py run.trace > run.trace.txt
      less run.trace.txt

