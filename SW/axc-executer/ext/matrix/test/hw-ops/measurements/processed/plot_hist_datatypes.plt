#!/usr/bin/env gnuplot -p
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set term pdf size 2.5, 3
set output "plot_hist_datatypes_".accel."_".msize.".pdf"

set xlabel "Mean Error Distance (%)"
set ylabel "Probability (%)"
set logscale x
set xrange [0.2:30]
set yrange [-1:30]

# hist_${HIST_BINS}_${HIST_PERC}_${Q_DIM}_${Q_WL}_${ACCEL}.data
# Type Kernel_Size Min Max Mean StdDev Variance
plot "norm_hist_500_100%_".msize."_4_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 4", \
     "norm_hist_500_100%_".msize."_6_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 6", \
     "norm_hist_500_100%_".msize."_8_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 8", \
     "norm_hist_500_100%_".msize."_10_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 10", \
     "norm_hist_500_100%_".msize."_12_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 12", \
     "norm_hist_500_100%_".msize."_14_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 14", \
     "norm_hist_500_100%_".msize."_16_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 16", \
     "norm_hist_500_100%_".msize."_18_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 18", \
     "norm_hist_500_100%_".msize."_20_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 20", \
     "norm_hist_500_100%_".msize."_22_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 22", \
     "norm_hist_500_100%_".msize."_24_".accel.".data" u ($0*0.2):($1*100) w l title "B_{data} = 24"
