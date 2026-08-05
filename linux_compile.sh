#!/bin/sh
# Compiles the linux platform layer with clang.
#
# @NOTE: this build links against the unversioned runtime libraries that ship
# with desktop linux (libasound.so.2, libX11.so.6, libEGL.so.1, libGL.so.1,
# libGLEW.so.2), so no -devel/-dev packages are required. GLEW headers come
# from the bundled lib/ and the ALSA API is hand-declared in linux.cpp.
#
# Usage:
#   sh linux_compile.sh          # just build
#   sh linux_compile.sh run      # build and run

set -e

mkdir -p build

clang++ -O0 -g -D IS_SERVER=0 \
    -DGLEW_NO_GLU \
    -I src \
    -I lib \
    src/platform/linux.cpp \
    -o build/game \
    -l:libEGL.so.1 -l:libGL.so.1 -l:libGLEW.so.2 -l:libX11.so.6 -l:libasound.so.2 \
    -lpthread -ldl -lm

if [ "$1" = "run" ]; then
    ./build/game
fi
