#!/usr/bin/env bash 

set -u

PIN_DOWNLOAD_URL="https://software.intel.com/sites/landingpage/pintool/downloads"
PIN_KIT_OSX="pin-2.13-61206-clang.3.0-mac"
PIN_KIT_WINDOWS="pin-2.13-61206-msvc10-windows"
PIN_KIT_LINUX="pin-2.13-61206-gcc.4.4.7-linux"

if [ $EUID -eq 0 ]; then
    echo "Please run this script as a regular user, not root."
    exit 1
fi

INSTALL_DIR="$PWD"
if [ $? -ge 1 ]; then
    INSTALL_DIR="$1"
    if test ! -d "$INSTALL_DIR" -o test ! -w "$INSTALL_DIR"; then
        echo "Error: install directory not writable: $INSTALL_DIR"
        exit 1
    fi
fi

SYSTEM=`uname -s`
case "$SYSTEM" in
    Darwin)  PIN_KIT="$PIN_KIT_OSX"     ;;
    CYGWIN*) PIN_KIT="$PIN_KIT_WINDOWS" ;;
    *)       PIN_KIT="$PIN_KIT_LINUX"   ;;
esac

echo "Installing PIN kit $PIN_KIT into: $INSTALL_DIR"

PIN_TGZ="${PIN_KIT}.tar.gz"
PIN_URL="${PIN_DOWNLOAD_URL}/${PIN_TGZ}"

function curl_get() {
    curl "$1" -o "$2"
}

function wget_get() {
   wget --progress "$1" -O "$2"
} 

if [ ! -d "${INSTALL_DIR}/${PIN_KIT}" ]; then
    if type -p curl 2>/dev/null; then
        DLFUN=curl_get
    elif type -p wget 2>/dev/null; then
        DL=wget_get
    else
        echo "Sorry, couldn't find a download program :("
        exit 1
    fi
    rm -f "${INSTALL_DIR}/${PIN_TGZ}"
    $DLFUN "$PIN_URL" "${INSTALL_DIR}/${PIN_TGZ}"
    tar x -C "${INSTALL_DIR}" -zf "${PIN_TGZ}"
fi

rm -f pin
ln -s "${INSTALL_DIR}/${PIN_KIT}" pin
PIN_ROOT="$PWD/pin"

cat >pin_env.sh <<END
export PIN_ROOT="$PIN_ROOT"
export PIN_HOME="\$PIN_ROOT"
export PATH="\$PATH:\$PIN_HOME"
END

echo "Created pin_env.sh with PIN_ROOT=$PIN_ROOT"

