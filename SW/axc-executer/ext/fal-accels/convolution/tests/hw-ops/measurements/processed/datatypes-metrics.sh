#!/bin/bash
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# "${Q_KS} ${Q_BW} ${Q_O} ${IMG_RMSE} ${IMG_PSNR} ${IMG_MEAN} ${IMG_STD} ${IMG_SSIM}"

# Join the images for the metrics
IMGS="baboon barbara lenna"
ACCELS="Winograd Spatial"
KS="3 5 7"
for img in ${IMGS};
do
  for acc in ${ACCELS};
  do
    FILE="stats_${acc}_${img}.data"
    for ks in ${KS};
    do
      awk "{if (\$1 == ${ks} && \$3 == 2) print \$0}" ${FILE} >> "stats_${acc}_${ks}_processed_.data"
    done
  done
done

# Compute the metrics
METRICS=(RMSE PSNR SSIM)
INDICES=(4 5 8)
for acc in ${ACCELS};
do
  for i in $(seq 0 1 2);
  do
    metric="${METRICS[$i]}"
    midx="${INDICES[$i]}"
    gnuplot -e "accel='${acc}';metric='${metric}';midx=${midx}" plot_metric.plt
  done
  gnuplot -e "accel='${acc}'" plot_mean_std.plt
done

