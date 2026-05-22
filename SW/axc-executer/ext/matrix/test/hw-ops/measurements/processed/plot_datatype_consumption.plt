#!/usr/bin/env gnuplot -p
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set term pdf size 2.5, 2.5
set output "plot_".msize."_".accel."_dtype_consumption.pdf"

set xlabel "Data width (bits)"
set ylabel "Relative consumption in % (xc7a50t)"
set xrange [4:24]
set yrange [0:25]
set y2range [0:10]
set xtics 4
set y2label "Delay in ns"
set y2tics nomirror
set ytics nomirror
set logscale y
#set key left top
set key above
#set nokey

# dims bw min_lat avg_lat max_lat dsp lut bram ff
plot "consumption_".accel."_".msize."_dtypes.data" u 2:($6*100) smooth unique w lp title "DSPs",\
     "consumption_".accel."_".msize."_dtypes.data" u 2:($7*100) smooth unique w lp title "LUTs",\
     "consumption_".accel."_".msize."_dtypes.data" u 2:($9*100) smooth unique w lp title "FFs",\
     "consumption_".accel."_".msize."_dtypes.data" u 2:($10) smooth unique w lp axis x1y2 title "Delay" 
#"consumption_".accel."_".msize."_dtypes.data" u 2:4 smooth unique w lp axis x1y2 title "Clock Cycles", \
#"consumption_".accel."_".msize."_dtypes.data" u 2:($8*100) smooth unique w lp title "BRAM",\

