#!/bin/bash

# Compute the maximum and then, divide
for file in $(ls hist_*);
do
  # Normalising
  echo "Normalising: ${file}"
  SUM=$(awk '{sum += $1; next} END {print sum}' ${file})
  awk "{print \$1/${SUM}}" ${file} > norm_${file}
done

# Compute the histograms for every accel and image
MIN_Q_DIM=${MIN_Q_DIM:-2}
MAX_Q_DIM=${MAX_Q_DIM:-2}
STEP_Q_DIM=${STEP_Q_DIM:-2}
MSIZES=$(seq ${MIN_Q_DIM} ${STEP_Q_DIM} ${MAX_Q_DIM})
ACCELS="matwinograd matstrassen matmul"
for acc in ${ACCELS};
do
  for msize in ${MSIZES};
  do
    gnuplot -e "accel='${acc}';msize='${msize}'" plot_hist_datatypes.plt
  done
done
