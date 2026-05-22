#!/bin/bash

LINK="https://www.dropbox.com/scl/fi/fcoh0t7zrekxhs8i9bb18/test.tar.gz?rlkey=qqde1t7hu331phaihcj14sp2g&dl=1"
DATA_TAR=./test.tar.gz
OUT_DIR=test
if ! [ -f ${DATA_TAR} ]; then
  wget ${LINK} -O ${DATA_TAR}
fi
mkdir -p ${OUT_DIR}
tar xzf ${DATA_TAR}

