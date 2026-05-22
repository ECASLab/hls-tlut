#!/bin/bash

set -e

# Prepare the directories
iterbuild=$1
cd ${iterbuild}

# Compile
if [ ! -e build ]; then
    meson setup build
fi
ninja -C build
