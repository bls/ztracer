#!/usr/bin/env bash

set -e

SYSTEM=$(uname -s)
ARCH=$(uname -m)

case "$SYSTEM" in
    Darwin) LIBEXT=       ;;
    *)      LIBEXT=.so     ;;
esac

case "$ARCH" in
    x86_64) LIBDIR=obj-intel64 ;;
    *)      LIBDIR=obj-ia32    ;;
esac

# Workaround Linux ptrace hardening if user elected not to
# allow non-child ptrace.
INJECTION=dynamic
if [ "$SYSTEM" == "Linux" ]; then
    PTRACE_FILE=/proc/sys/kernel/yama/ptrace_scope
    if [ -r "$PTRACE_FILE" ]; then
        if [ $(cat $PTRACE_FILE) -eq 1 ]; then
            INJECTION=child
        fi
    fi
fi

pin -injection "$INJECTION" -follow_execv 1 -separate_memory 1 \
    -t "${LIBDIR}/ZTracer${LIBEXT}" -- $@;

