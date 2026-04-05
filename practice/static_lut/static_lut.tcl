catch {::common::set_param -quiet hls.xocc.mode csynth};
set part "xck26-sfvc784-2LV-c"
open_project static_lut_hls
set_top acelerador_nolineal
add_files "static_lut.cpp"
open_solution -flow_target vitis solution1
set_part $part
create_clock -period 100MHz -name default
config_dataflow -strict_mode warning
config_rtl -deadlock_detection sim
config_interface -m_axi_conservative_mode=1
config_interface -m_axi_addr64
config_interface -m_axi_auto_max_ports=0
config_export -format xo -ipname acelerador_nolineal
add_files -tb static_lut_tb.cpp
csim_design
csynth_design
close_project
puts "HLS completed successfully para el flujo Kria K26"
exit