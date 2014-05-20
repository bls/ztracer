#!/bin/bash

PTRACE_SYSCTL=kernel.yama.ptrace_scope
PTRACE_CONF=/etc/sysctl.d/10-ptrace.conf

if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root."
    exit 1
fi

permanently_fix_ptrace() {
    if [[ -f "$PTRACE_CONF" ]]; then
        mv -i "$PTRACE_CONF" "${PTRACE_CONF}.old.$$"
        echo "${PTRACE_SYSCTL}=0" > "$PTRACE_CONF"
        service procps start
    else
        echo "Couldn't find file: $PTRACE_CONF"
        echo "Unable to permanently disable ptrace :("
    fi
}

temporarily_fix_ptrace() {
    sysctl $PTRACE_SYSCTL=0
}

main() {
    if test `sysctl -n $PTRACE_SYSCTL` -eq 0; then
        sysctl $PTRACE_SYSCTL
        echo "Looks like ptrace is not restricted, no action required"
        exit 0
    fi
    
cat <<EOM

    Ptrace is restricted on this system (only child processes can
    be traced). PIN will function better if you disabled this. 
 
    Do you want to enable ptrace?   Select one of the following:

    "p" -> enable ptrace permanently
    "t" -> enable ptrace temporarily (lasts until reboot)
    
    any other response will result in no change to your system.
 
EOM
    read INPUT
    case "$INPUT" in
        p) permanently_fix_ptrace ;;
        t) temporarily_fix_ptrace ;;
        *) echo "Not changing ptrace setting at this time" ;;
    esac
}

main

