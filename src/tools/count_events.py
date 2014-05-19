#!/usr/bin/env python

import os, sys
import ztrace

if __name__ == '__main__':
    tr = ztrace.TraceFileReader(sys.argv[1])
    print tr.count_events()

