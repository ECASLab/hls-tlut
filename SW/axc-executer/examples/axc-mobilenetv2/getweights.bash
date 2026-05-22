#!/bin/bash

LINK="https://www.dropbox.com/scl/fi/cbts6hmu1ea9k8l6z1vy7/parameters.tar.gz?rlkey=k478lrvfeampu45mlp6fs2s15&dl=1"
WEIGHTS_TAR=./weights.tar.gz
OUT_DIR=weights
if ! [ -f ${WEIGHTS_TAR} ]; then
  wget ${LINK} -O ${WEIGHTS_TAR}
fi
mkdir ${OUT_DIR}
tar xzf ${WEIGHTS_TAR} -C ${OUT_DIR}
