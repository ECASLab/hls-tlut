#!/usr/bin/env gnuplot -p
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

set term svg size 500, 400
set output "plot_k".mkernel."_dtype_efficiency.svg"

set xlabel "Datatype bit-width (bits)"
set ylabel "Design Efficiency"
set key right top

space_ops(k,o)=(k*k*o)
winograd_ops(k,i,o)=((i*i*o + o*o*i)+(2*i*i*i)+(i*k*k + i*i*k))
freq=100000000
max(w,v)=w>v?w:v
div=1000000000
peakperf=276

#ksize bw o min_lat avg_lat max_lat dsp lut bram ff
plot "consumption_Spatial_".mkernel."_2_dtypes.data" u 2:((space_ops(3,2) * freq / $5) * floor(1 / max($8,$7)) / div / peakperf) smooth unique w lp title "Space O = 2",\
     "consumption_Spatial_".mkernel."_4_dtypes.data" u 2:((space_ops(3,4) * freq / $5) * floor(1 / max($8,$7)) / div / peakperf) smooth unique w lp title "Space O = 4",\
     "consumption_Spatial_".mkernel."_6_dtypes.data" u 2:((space_ops(3,6) * freq / $5) * floor(1 / max($8,$7)) / div / peakperf) smooth unique w lp title "Space O = 6",\
     "consumption_Spatial_".mkernel."_8_dtypes.data" u 2:((space_ops(3,8) * freq / $5) * floor(1 / max($8,$7)) / div / peakperf) smooth unique w lp title "Space O = 8",\
     "consumption_Winograd_".mkernel."_dtypes.data" u 2:((winograd_ops(3,4,2) * freq / $5) * floor(1 / max($8,$7)) / div / peakperf) smooth unique w lp title "Winograd O = 2"
