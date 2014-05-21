#!/usr/bin/env bash

set -e

if [[ $EUID -eq 0 ]]; then
    echo "This script must NOT be run as root."
    exit 1
fi

if [ ! which brew ]; then
    echo "This script requires 'homebrew' to be installed."
    echo "Get it from here: http://brew.sh/"
    exit 1
fi

if [ ! which easy_install ]; then
    echo "This script requires 'easy_install' for Python."
    echo "Get it from here: https://pypi.python.org/pypi/setuptools"
    exit 1
fi

echo "Installing system dependencies via homebrew..."
brew install protobuf log4cpp snappy

echo "Installing Python dependencies (requires root!)"
sudo easy_install protobuf 
DAMMIT="-Wno-error=unused-command-line-argument-hard-error-in-future"
sudo ARCHFLAGS="$DAMMIT" easy_install python-snappy

