#!/usr/bin/env bash

set -e

SYSTEM=$(uname -s)

case "$SYSTEM" in
    Linux)  
       sudo ./scripts/ubuntu/install_deps.sh
       ;;
    Darwin) 
       ./scripts/osx/install_deps.sh
       ;;
    *) 
       echo "Unsupported operating system :("
       exit 1
       ;;
esac

