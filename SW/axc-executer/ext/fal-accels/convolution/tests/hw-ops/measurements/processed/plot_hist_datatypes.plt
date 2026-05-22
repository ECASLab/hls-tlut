#!/usr/bin/env gnuplot -p
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set term svg size 640, 480
set output "plot_hist_datatypes_".accel."_".img."_".kernel.".svg"

set xlabel "Normalised Error (0 to 1)"
set ylabel "Scaled occurence (1/max)"

# Type Kernel_Size Min Max Mean StdDev Variance
plot "norm_hist_500_30%_".img."_4_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 4", \
     "norm_hist_500_30%_".img."_6_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 6", \
     "norm_hist_500_30%_".img."_8_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 8", \
     "norm_hist_500_30%_".img."_10_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 10", \
     "norm_hist_500_30%_".img."_12_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 12", \
     "norm_hist_500_30%_".img."_14_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 14", \
     "norm_hist_500_30%_".img."_16_".accel."_".kernel.".data" u ($0*0.3/500):($1) w l title "W = 16"
