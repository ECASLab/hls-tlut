#!/usr/bin/env gnuplot -p
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set term svg size 640, 480
set output "plot_".ks."_".accel."_output_consumption.svg"

set xlabel "Output size (NxN)"
set ylabel "Relative consumption (Avnet Zedboard - ZYNQ 7z020) - %"
set y2label "Latency in clocks"
set y2tics nomirror
set ytics nomirror
set style fill solid
set boxwidth 0.2
set xtics 2
set key left top

# ksize bw o min_lat avg_lat max_lat dsp lut bram ff
plot "consumption_".accel."_".ks."_outputs.data" u ($3-0.4):($7*100) with boxes title "DSPs",\
     "consumption_".accel."_".ks."_outputs.data" u ($3-0.2):($8*100) with boxes title "LUTs",\
     "consumption_".accel."_".ks."_outputs.data" u 3:($9*100) with boxes title "BRAM",\
     "consumption_".accel."_".ks."_outputs.data" u ($3+0.2):($10*100) with boxes title "FFs",\
     "consumption_".accel."_".ks."_outputs.data" u ($3+0.4):5 with boxes axis x1y2 title "Avg Latency"

