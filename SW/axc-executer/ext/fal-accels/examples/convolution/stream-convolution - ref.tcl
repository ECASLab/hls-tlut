############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Register Mappings
# set_directive_array_partition -type complete -dim 0 "convolver_accel_top_accel" input
# set_directive_array_partition -type complete -dim 0 "convolver_accel_top_accel" kernel
# set_directive_array_partition -type complete -dim 0 "convolver_accel_top_accel" output

# Register loop optimisation - set 1
#set_directive_pipeline "Spatial<T, K>::Execute/space_exact_execute_i"
#set_directive_loop_flatten "Spatial<T, K>::Execute/space_exact_execute_r"

# Register loop optimisation - set 2
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_i"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_j"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_e"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_r"

# Important! Inlining the execution allows parallelism
set_directive_inline "Spatial<T, K>::Execute"
set_directive_inline "mult"

# Data binding
set_directive_interface -mode s_axilite -register -offset 0x100 "$::env(TOP_FUNCTION)" input_width_rd
set_directive_interface -mode s_axilite -register -offset 0x108 "$::env(TOP_FUNCTION)" input_height_rd
set_directive_interface -mode s_axilite -register -offset 0x110 "$::env(TOP_FUNCTION)" output_width_rd
set_directive_interface -mode s_axilite -register -offset 0x118 "$::env(TOP_FUNCTION)" output_height_rd
set_directive_interface -mode s_axilite -register -offset 0x120 "$::env(TOP_FUNCTION)" kernel_size_rd
set_directive_interface -mode s_axilite -register -offset 0x128 "$::env(TOP_FUNCTION)" num_kernels_rd
set_directive_interface -mode s_axilite -register -offset 0x130 "$::env(TOP_FUNCTION)" output_scaling_rd
set_directive_interface -mode s_axilite -register -offset 0x138 "$::env(TOP_FUNCTION)" padding_type_rd
set_directive_interface -mode s_axilite -register -offset 0x140 "$::env(TOP_FUNCTION)" stride_x_rd
set_directive_interface -mode s_axilite -register -offset 0x148 "$::env(TOP_FUNCTION)" stride_y_rd
set_directive_interface -mode s_axilite -register -offset 0x150 "$::env(TOP_FUNCTION)" dilatation_x_rd
set_directive_interface -mode s_axilite -register -offset 0x158 "$::env(TOP_FUNCTION)" dilatation_y_rd
set_directive_interface -mode s_axilite -register -offset 0x160 "$::env(TOP_FUNCTION)" execution_mode_rd
set_directive_interface -mode s_axilite -register -offset 0x200 "$::env(TOP_FUNCTION)" input_width_wr
set_directive_interface -mode s_axilite -register -offset 0x208 "$::env(TOP_FUNCTION)" input_height_wr
set_directive_interface -mode s_axilite -register -offset 0x210 "$::env(TOP_FUNCTION)" output_width_wr
set_directive_interface -mode s_axilite -register -offset 0x218 "$::env(TOP_FUNCTION)" output_height_wr
set_directive_interface -mode s_axilite -register -offset 0x220 "$::env(TOP_FUNCTION)" kernel_size_wr
set_directive_interface -mode s_axilite -register -offset 0x228 "$::env(TOP_FUNCTION)" num_kernels_wr
set_directive_interface -mode s_axilite -register -offset 0x230 "$::env(TOP_FUNCTION)" output_scaling_wr
set_directive_interface -mode s_axilite -register -offset 0x238 "$::env(TOP_FUNCTION)" padding_type_wr
set_directive_interface -mode s_axilite -register -offset 0x240 "$::env(TOP_FUNCTION)" stride_x_wr
set_directive_interface -mode s_axilite -register -offset 0x248 "$::env(TOP_FUNCTION)" stride_y_wr
set_directive_interface -mode s_axilite -register -offset 0x250 "$::env(TOP_FUNCTION)" dilatation_x_wr
set_directive_interface -mode s_axilite -register -offset 0x258 "$::env(TOP_FUNCTION)" dilatation_y_wr
set_directive_interface -mode s_axilite -register -offset 0x260 "$::env(TOP_FUNCTION)" execution_mode_wr

# Accelerator capabilities
set_directive_interface -mode s_axilite -register -offset 0x300 "$::env(TOP_FUNCTION)" input_width_min
set_directive_interface -mode s_axilite -register -offset 0x308 "$::env(TOP_FUNCTION)" input_width_max
set_directive_interface -mode s_axilite -register -offset 0x310 "$::env(TOP_FUNCTION)" input_height_min
set_directive_interface -mode s_axilite -register -offset 0x318 "$::env(TOP_FUNCTION)" input_height_max
set_directive_interface -mode s_axilite -register -offset 0x320 "$::env(TOP_FUNCTION)" output_width_min
set_directive_interface -mode s_axilite -register -offset 0x328 "$::env(TOP_FUNCTION)" output_width_max
set_directive_interface -mode s_axilite -register -offset 0x330 "$::env(TOP_FUNCTION)" output_height_min
set_directive_interface -mode s_axilite -register -offset 0x338 "$::env(TOP_FUNCTION)" output_height_max
set_directive_interface -mode s_axilite -register -offset 0x340 "$::env(TOP_FUNCTION)" scaling_min
set_directive_interface -mode s_axilite -register -offset 0x348 "$::env(TOP_FUNCTION)" scaling_max
set_directive_interface -mode s_axilite -register -offset 0x350 "$::env(TOP_FUNCTION)" stride_cap
set_directive_interface -mode s_axilite -register -offset 0x358 "$::env(TOP_FUNCTION)" dilatation_cap
set_directive_interface -mode s_axilite -register -offset 0x360 "$::env(TOP_FUNCTION)" padding_cap
set_directive_interface -mode s_axilite -register -offset 0x368 "$::env(TOP_FUNCTION)" bias_cap
set_directive_interface -mode s_axilite -register -offset 0x370 "$::env(TOP_FUNCTION)" num_inputs_cap
set_directive_interface -mode s_axilite -register -offset 0x378 "$::env(TOP_FUNCTION)" num_kernels_cap
set_directive_interface -mode s_axilite -register -offset 0x380 "$::env(TOP_FUNCTION)" kernel_size_cap
set_directive_interface -mode s_axilite -register -offset 0x388 "$::env(TOP_FUNCTION)" num_cores_cap
set_directive_interface -mode s_axilite -register -offset 0x390 "$::env(TOP_FUNCTION)" datatype_cap
set_directive_interface -mode s_axilite -register -offset 0x398 "$::env(TOP_FUNCTION)" num_bits_integer_cap
set_directive_interface -mode s_axilite -register -offset 0x3A0 "$::env(TOP_FUNCTION)" num_bits_fraction_cap

set_directive_interface -mode s_axilite -register -offset 0x500 "$::env(TOP_FUNCTION)" kernels

set_directive_interface -mode s_axilite -register -offset 0x600 "$::env(TOP_FUNCTION)" debug_0_rd
set_directive_interface -mode s_axilite -register -offset 0x608 "$::env(TOP_FUNCTION)" debug_0_wr
set_directive_interface -mode s_axilite -register -offset 0x610 "$::env(TOP_FUNCTION)" debug_1_rd
set_directive_interface -mode s_axilite -register -offset 0x618 "$::env(TOP_FUNCTION)" debug_1_wr
set_directive_interface -mode s_axilite -register -offset 0x620 "$::env(TOP_FUNCTION)" debug_2_rd
set_directive_interface -mode s_axilite -register -offset 0x628 "$::env(TOP_FUNCTION)" debug_2_wr
set_directive_interface -mode s_axilite -register -offset 0x630 "$::env(TOP_FUNCTION)" debug_3_rd
set_directive_interface -mode s_axilite -register -offset 0x638 "$::env(TOP_FUNCTION)" debug_3_wr

set_directive_interface -mode s_axilite -register -offset 0x800 "$::env(TOP_FUNCTION)" accel_type_cap
set_directive_interface -mode s_axilite -register -offset 0x808 "$::env(TOP_FUNCTION)" accel_version_cap

# Stream port and ctrl bindings
set_directive_interface -mode s_axilite -bundle ctrl "$::env(TOP_FUNCTION)"
set_directive_interface -mode axis -register -register_mode both "$::env(TOP_FUNCTION)" stream_input
set_directive_interface -mode axis -register -register_mode both "$::env(TOP_FUNCTION)" stream_output
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)" kernels

# -----------------------------------------------------------------------------
# Space
# -----------------------------------------------------------------------------

# Register loop optimisation - set 2
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_i"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_j"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_e"
set_directive_unroll "Spatial<T, K>::Execute/space_exact_execute_r"

# Important! Inlining the execution allows parallelism
set_directive_inline "Spatial<T, K>::Execute"
set_directive_inline "mult"

# -----------------------------------------------------------------------------
# Winograd
# -----------------------------------------------------------------------------

# Register Hadamard optimisation
set_directive_unroll "Winograd<T, K>::Hadamard/winograd_exact_hadamard_i"
set_directive_unroll "Winograd<T, K>::Hadamard/winograd_exact_hadamard_j"

# Optimisation
set_directive_inline -off "Winograd<T, K>::Execute"
