#!/bin/sh

SYSTEM=`uname -s`
ARCH=`uname -m`

case "$SYSTEM" in
    Darwin) LIBEXT=       ;;
    *)      LIBEXT=.so     ;;
esac

case "$ARCH" in
    x86_64) LIBDIR=obj-intel64 ;;
    *)      LIBDIR=obj-ia32    ;;
esac

pin -follow_execv 1 -separate_memory 1 -t "${LIBDIR}/ZTracer${LIBEXT}" -- $@;


