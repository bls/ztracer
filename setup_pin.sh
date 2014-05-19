#!/bin/bash

PIN_DOWNLOAD_URL="http://software.intel.com/sites/landingpage/pintool/downloads"
PIN_KIT_OSX="pin-2.13-61206-clang.3.0-mac"
PIN_KIT_WINDOWS="pin-2.13-61206-msvc10-windows"
PIN_KIT_LINUX="pin-2.13-61206-gcc.4.4.7-linux"

if [[ $EUID -eq 0 ]]; then
    echo "Please run this script as a regular user, not root."
    exit 1
fi

SYSTEM=`uname -s`

case "$SYSTEM" in
    Darwin)  PIN_KIT="$PIN_KIT_OSX"     ;;
    CYGWIN*) PIN_KIT="$PIN_KIT_WINDOWS" ;;
    *)       PIN_KIT="$PIN_KIT_LINUX"   ;;
esac

PIN_TGZ="${PIN_KIT}.tar.gz"
PIN_URL="${PIN_DOWNLOAD_URL}/${PIN_TGZ}"

if [[ ! -d "$PIN_KIT" ]]; then
    if type -p wget 2>/dev/null; then
        DL=wget
    elif type -p curl 2>/dev/null; then
        DL=curl
    else
        echo "Sorry, couldn't find a download program :("
        exit 1
    fi
    rm -f "$PIN_TGZ"
    $DL "$PIN_URL"
    tar xzf "$PIN_TGZ"
fi

PIN_ROOT="$PWD/$PIN_KIT"

cat >pin_env.sh <<END
export PIN_ROOT="$PIN_ROOT"
export PIN_HOME="\$PIN_ROOT"
export PATH="\$PATH:\$PIN_HOME"
END

echo "Created pin_env.sh with PIN_ROOT=$PIN_ROOT"
