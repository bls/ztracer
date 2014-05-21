#!/usr/bin/env bash

source ./pin_env.sh
cd src
# So the Apple workers for travis-ci seem to vary quite a lot
# in terms of the way they are set up. We *need* to use clang
# to build PIN tools on OSX.
if [ $(uname -s) == "Darwin" ]; then
    export CC=clang
    export CXX=clang
fi
make

