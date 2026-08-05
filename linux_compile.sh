#!/bin/sh
# Compiles the linux platform layer with clang.
#
# Required packages (Debian/Ubuntu):
#   clang libx11-dev libegl1-mesa-dev libgl1-mesa-dev libglew-dev libasound2-dev
#
# @NOTE: on linux we use the system GLEW, not the bundled one in lib/ (that one
# is windows-only). System headers are found automatically; no -I lib here.
#
# Usage:
#   sh linux_compile.sh          # just build
#   sh linux_compile.sh run      # build and run

set -e

mkdir -p build

clang++ -O0 -g -D IS_SERVER=0 \
    src/platform/linux.cpp \
    -o build/game \
    -lEGL -lGL -lGLEW -lX11 -lasound -lpthread -ldl -lm

if [ "$1" = "run" ]; then
    ./build/game
fi
