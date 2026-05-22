#!/bin/bash
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# ksize bw o min_lat avg_lat max_lat dsp lut bram ff

# Join the consumptions for datatypes
OUTPUT="2"
KS="3 5 7"
ACCELS="Spatial Winograd"
for acc in ${ACCELS};
do
  FILE="${acc}-report.data"
  for ks in ${KS};
  do
    awk "{if (\$1 == ${ks} && \$3 == ${OUTPUT}) print \$0}" ${FILE} > "consumption_${acc}_${ks}_dtypes.data"
    gnuplot -e "accel='${acc}';ks='${ks}'" plot_datatype_consumption.plt
  done
done

ACCELS="Spatial"
for acc in ${ACCELS};
do
  FILE="${acc}-report.data"
  for o in $(seq 2 2 8);
  do
    for ks in ${KS};
    do
      awk "{if (\$1 == ${ks} && \$3 == ${o}) print \$0}" ${FILE} > "consumption_${acc}_${ks}_${o}_dtypes.data"
    done
  done
done

for ks in ${KS};
do
  gnuplot -e "accel='${acc}';mkernel='${ks}'" plot_datatype_efficiency.plt
  #gnuplot -e "accel='${acc}';mkernel='${ks}'" plot_datatype_performance.plt
done

# Join the consumptions for output
DATATYPE=8
for acc in ${ACCELS};
do
  FILE="${acc}-report.data"
  for ks in ${KS};
  do
    awk "{if (\$1 == ${ks} && \$2 == ${DATATYPE}) print \$0}" ${FILE} > "consumption_${acc}_${ks}_outputs.data"
    gnuplot -e "accel='${acc}';ks='${ks}'" plot_output_consumption.plt
  done
done
