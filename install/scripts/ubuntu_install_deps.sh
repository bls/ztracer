#!/bin/bash

BASE_PKGS="build-essential liblog4cpp5-dev libprotobuf-dev protobuf-compiler"
BASE_PKGS="$BASE_PKGS libsnappy-dev python-snappy python-protobuf"

if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root."
    exit 1
fi

###
# PIN on 64-bit Ubuntu requires 32-bit compatibility libraries.
# This used to be ia32-libs, but that all changed in Ubuntu 13.10
# with multiarch.
###
install_i386_compat_for_x64() {
    local RELEASE=`lsb_release -sr`
    local ARCH=`uname -m`
    local RELEASE_MAJOR=`echo $RELEASE | cut -d. -f1`
    local RELEASE_MINOR=`echo $RELEASE | cut -d. -f2`

    if [[ "$ARCH" -eq "x86_64" ]]; then
        if [[ "$RELEASE_MAJOR" -gt 13 ]]; then
            install_multiarch_compat
        elif [[ "$RELEASE_MAJOR" -eq 13 && "$RELEASE_MINOR" -ge 10 ]]; then
            install_multiarch_compat
        else
            install_oldschool_compat
        fi
    else
       echo 
    fi
}

install_multiarch_compat() {
    dpkg --add-architecture i386
    apt-get update
    sudo apt-get install libc6:i386 libstdc++6:i386
}

install_oldschool_compat() {
    apt-get install ia32-libs
}

apt-get install $BASE_PKGS
install_i386_compat_for_x64

