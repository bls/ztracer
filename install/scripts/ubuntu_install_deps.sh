#!/bin/bash

BASE_PKGS="build-essential liblog4cpp5-dev libprotobuf-dev libsnappy-dev python-snappy python-protobuf"
NEW_COMPAT_PKGS="libc6-i386 libstdc++6:i386"
OLD_COMPAT_PKGS="ia32-libs"

if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root."
    exit 1
fi

###
# PIN on 64-bit Ubuntu requires 32-bit compatibility libraries.
# This used to be ia32-libs, but that all changed in Ubuntu 13.10
# with multiarch.
###
compat_packages_for_64bit() {
    local RELEASE=`lsb_release -sr`
    local ARCH=`uname -m`
    local RELEASE_MAJOR=`echo $RELEASE | cut -d. -f1`
    local RELEASE_MINOR=`echo $RELEASE | cut -d. -f2`

    if [[ "$ARCH" -eq "x86_64" ]]; then
        if [[ "$RELEASE_MAJOR" -ge 13 && "$RELEASE_MINOR" -ge 10 ]]; then
            echo $NEW_COMPAT_PKGS
        else
            echo $OLD_COMPAT_PKGS
        fi
    else
       echo 
    fi
}

main() {
    local COMPAT_PKGS=`compat_packages_for_64bit`

cat <<EOM
    About to install the following packages:

    $COMPAT_PKGS
    $BASE_PKGS

    Press Ctrl-C to cancel or [Enter] to proceed.
EOM

    read
    apt-get install $COMPAT $BASE_PKGS
}

main

