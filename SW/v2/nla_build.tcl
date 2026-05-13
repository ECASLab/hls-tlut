# ----------------------------------------------------------------------------
# Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
# ----------------------------------------------------------------------------

open_project nla_proj_hls
set_top nla_top
add_files nla_core.cpp
add_files -tb nla_tb.cpp
open_solution -reset "solution1" -flow_target vitis
set_part {xck26-sfvc784-2LV-c}
create_clock -period 250MHz -name default
config_dataflow -strict_mode warning
csim_design
csynth_design
cosim_design

# Limpieza y Consolidación Total
set xml_files [glob -nocomplain nla_proj_hls/solution1/syn/report/*.xml nla_proj_hls/solution1/sim/report/*.xml]
if {[llength $xml_files] > 0} { file delete {*}$xml_files }

set master_report "nla_proj_hls/solution1/MASTER_REPORT.txt"
set out_channel [open $master_report w]
set rpt_files [glob -nocomplain nla_proj_hls/solution1/syn/report/*.rpt nla_proj_hls/solution1/sim/report/*.rpt]

foreach f $rpt_files {
    puts $out_channel "================================================================"
    puts $out_channel " REPORTE: [file tail $f]"
    puts $out_channel "================================================================"
    set in_channel [open $f r]
    puts $out_channel [read $in_channel]
    close $in_channel
    puts $out_channel "\n\n"
}
close $out_channel
exit