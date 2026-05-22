#!/usr/bin/env sh

# this script is used to collect data from Vivado reports, it goes together
# with `create_datasets.sh`
#
# this script requires OPS_PATH to be defined, and be where the
# approximate-math-operators repository is contained

# IMPORTANT WARNING:
# this script runs git clean, so DO NOT RUN IT if you have unstaged changes
# that are important

# MPI
let CID=${OMPI_COMM_WORLD_RANK:-0}
let SIZE=${OMPI_COMM_WORLD_SIZE:-1}

set -xe

#git clean -dfx

tests="axc_add axc_multiply"

# Precision
MIN_Q_WL=${MIN_Q_WL:-6}
MAX_Q_WL=${MAX_Q_WL:-24}
STEP_Q_WL=${STEP_Q_WL:-2}

bits="1 2 3 4"
methods="EXACT DROP OR FIXED"
samples="50000"

# Offsets
START_IDX=$((${CID}*${STEP_Q_WL}+${MIN_Q_WL}))
STEP_IDX=$((${STEP_Q_WL}*${SIZE}))

for t in $tests; do
  export t=$t
  for z in $(seq ${START_IDX} ${STEP_IDX} ${MAX_Q_WL}); do
    export z=$z
    for b in $bits; do
      export b=$b
      for m in $methods; do
        export m=$m
        for s in $samples; do
          export s=$s
          Q_BW="$z" Q_INT=2 Q_O="$s" TEST="$t" AXC_BITS="$b" METHOD="$m" make
          unset s
        done
        unset m
      done
      unset b
    done
    unset z
  done
  unset t
done
