#!/bin/bash

INIT_START_PERMUTATION=6400
NUM_PERMUTATIONS=2700000
PERMUTATIONS_PER_RUN=12000

echo "Starting script. Waiting a couple of hours"
for START in `seq ${INIT_START_PERMUTATION} ${PERMUTATIONS_PER_RUN} ${NUM_PERMUTATIONS}`; do
    END=`expr ${START} + ${PERMUTATIONS_PER_RUN}`
    echo "Job batch: ${START} -> ${END}"
    echo -n ${START} > start.data
    echo -n ${END} > end.data
    sbatch fal-helper.sh
    sleep 6500
done