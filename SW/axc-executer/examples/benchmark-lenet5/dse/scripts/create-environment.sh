#!/bin/bash

# Prepare the directories
#construction="$(date '+%Y-%m-%d-%H-%M-%S-%N')-$1"
iterbuild="${HOME}/dse-iters/build-$1"
sourcedir="../../../"
if [ ! -e ${iterbuild} ]; then
    mkdir -p ${iterbuild}
    cp -ra ${sourcedir} ${iterbuild}
fi
# Copy sources
echo -n $iterbuild
