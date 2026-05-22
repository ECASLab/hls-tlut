############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Data binding
set_directive_interface -mode s_axilite -register -offset 0x100 "$::env(TOP_FUNCTION)" rows_rd
set_directive_interface -mode s_axilite -register -offset 0x108 "$::env(TOP_FUNCTION)" columns_rd
set_directive_interface -mode s_axilite -register -offset 0x110 "$::env(TOP_FUNCTION)" load_matrix_mask_rd
set_directive_interface -mode s_axilite -register -offset 0x118 "$::env(TOP_FUNCTION)" en_accumulation_rd
set_directive_interface -mode s_axilite -register -offset 0x120 "$::env(TOP_FUNCTION)" en_activation_rd
set_directive_interface -mode s_axilite -register -offset 0x128 "$::env(TOP_FUNCTION)" scaling_rd
set_directive_interface -mode s_axilite -register -offset 0x130 "$::env(TOP_FUNCTION)" operation_rd
set_directive_interface -mode s_axilite -register -offset 0x160 "$::env(TOP_FUNCTION)" execution_mode_rd

set_directive_interface -mode s_axilite -register -offset 0x200 "$::env(TOP_FUNCTION)" rows_wr
set_directive_interface -mode s_axilite -register -offset 0x208 "$::env(TOP_FUNCTION)" columns_wr
set_directive_interface -mode s_axilite -register -offset 0x210 "$::env(TOP_FUNCTION)" load_matrix_mask_wr
set_directive_interface -mode s_axilite -register -offset 0x218 "$::env(TOP_FUNCTION)" en_accumulation_wr
set_directive_interface -mode s_axilite -register -offset 0x220 "$::env(TOP_FUNCTION)" en_activation_wr
set_directive_interface -mode s_axilite -register -offset 0x228 "$::env(TOP_FUNCTION)" scaling_wr
set_directive_interface -mode s_axilite -register -offset 0x230 "$::env(TOP_FUNCTION)" operation_wr
set_directive_interface -mode s_axilite -register -offset 0x260 "$::env(TOP_FUNCTION)" execution_mode_wr

# Accelerator capabilities
set_directive_interface -mode s_axilite -register -offset 0x300 "$::env(TOP_FUNCTION)" rows_min
set_directive_interface -mode s_axilite -register -offset 0x308 "$::env(TOP_FUNCTION)" rows_max
set_directive_interface -mode s_axilite -register -offset 0x310 "$::env(TOP_FUNCTION)" columns_min
set_directive_interface -mode s_axilite -register -offset 0x318 "$::env(TOP_FUNCTION)" columns_max
set_directive_interface -mode s_axilite -register -offset 0x320 "$::env(TOP_FUNCTION)" scaling_min
set_directive_interface -mode s_axilite -register -offset 0x328 "$::env(TOP_FUNCTION)" scaling_max
set_directive_interface -mode s_axilite -register -offset 0x330 "$::env(TOP_FUNCTION)" masking_cap
set_directive_interface -mode s_axilite -register -offset 0x338 "$::env(TOP_FUNCTION)" accumulation_cap
set_directive_interface -mode s_axilite -register -offset 0x340 "$::env(TOP_FUNCTION)" activation_cap
set_directive_interface -mode s_axilite -register -offset 0x348 "$::env(TOP_FUNCTION)" def_scaling_cap
set_directive_interface -mode s_axilite -register -offset 0x350 "$::env(TOP_FUNCTION)" operations_cap

set_directive_interface -mode s_axilite -register -offset 0x388 "$::env(TOP_FUNCTION)" num_cores_cap
set_directive_interface -mode s_axilite -register -offset 0x390 "$::env(TOP_FUNCTION)" datatype_cap
set_directive_interface -mode s_axilite -register -offset 0x398 "$::env(TOP_FUNCTION)" num_bits_integer_cap
set_directive_interface -mode s_axilite -register -offset 0x3A0 "$::env(TOP_FUNCTION)" num_bits_fraction_cap

# Stream port and ctrl bindings
set_directive_interface -mode s_axilite -bundle ctrl "$::env(TOP_FUNCTION)"
set_directive_interface -mode axis -register -register_mode both "$::env(TOP_FUNCTION)" stream_input
set_directive_interface -mode axis -register -register_mode both "$::env(TOP_FUNCTION)" stream_output

set_directive_interface -mode s_axilite -register -offset 0x800 "$::env(TOP_FUNCTION)" accel_type_cap
set_directive_interface -mode s_axilite -register -offset 0x808 "$::env(TOP_FUNCTION)" accel_version_cap

# Unroll and pipeline for boosting speed and avoid serialisation
set_directive_pipeline "execute"
set_directive_unroll "execute/execute_rows_buffer"
set_directive_unroll "execute/execute_cols_buffer"
set_directive_pipeline "accel_def

# Interface
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)/accel_def" A
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)/accel_def" B
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)/accel_def" C

# Buffer partitioning
set_directive_array_partition -type complete -dim 0 "execute" D
set_directive_array_partition -type complete -dim 0 "retrieve_data" output_mat
set_directive_array_partition -type complete -dim 0 "receive_matrix" input_mat

# Data retrieval
set_directive_pipeline "retrieve_data"
set_directive_pipeline "load_data"
set_directive_inline "receive_matrix"

# Inline for boosting vectorisation
set_directive_inline "MatrixMultiplyAdd<T, M, N, ADD, MULT, NL>::Execute"
