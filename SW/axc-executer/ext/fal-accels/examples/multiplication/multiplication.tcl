############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Custom inlining (disable) - you can add any required
#set_directive_inline -off "multiply"

# It is recommended to avoid placing pragmas within the code
# for optimisation flexibility and DSE

# Original baseline
# Load Consumption: 325 FF 1197 LUT 155 Cycle
# Execute Consumption: 2 DSP 380 FF 1144 LUT 251 Cycle
# WriteBack Consumption: 162 FF 1286 LUT 50 Cycle
# Note: everything is inlined

# Load function optimisation - Step 1 
# Load Consumption: 1517 FF 1449 LUT 35 Cycle
set_directive_pipeline "ReceiveMatrix/data_stream_loop_packet"
set_directive_unroll "ReceiveMatrix/data_stream_loop_element"
set_directive_unroll "ReceiveMatrix/data_stream_copy_row"
set_directive_unroll "ReceiveMatrix/data_stream_copy_col"

# Load function optimisation - Step 2
# Load Consumption: 1479 FF 1143 LUT 13 Cycle
set_directive_array_partition -type complete -dim 0 "ReceiveMatrix" input_mat

# WriteBack function optimisation - Step 1
# WriteBack Consumption: 201 FF 288 LUT 11 Cycle
set_directive_array_partition -type complete -dim 0 "WriteBack" output_mat
set_directive_unroll "WriteBack/data_retrieve_copy_row"
set_directive_unroll "WriteBack/data_retrieve_copy_col"
set_directive_pipeline "WriteBack/data_retrieve_packet"
set_directive_unroll "WriteBack/data_retrieve_elements"

# WriteBack function optimisation - Step 2
# WriteBack Consumption: 82 FF 205 LUT 4 Cycle
# Note: Aggresive partitioning. It could be reshaped in 1st dim
set_directive_array_partition -type complete -dim 0 "multiplication_accel_top" C

# Execute function optimisation - Step 1
# Execute Consumption: 16 DSP 911 FF 698 LUT 5 Cycle
set_directive_unroll -skip_exit_check "Execute/clear_pe_matrix_port"
set_directive_loop_flatten "Execute/clear_pe_matrix_port_cols"
set_directive_unroll -skip_exit_check "Execute/clear_pe_matrix_port_cols"
set_directive_loop_flatten -off "Execute/clear_pe_matrix_port_rows"
set_directive_unroll -skip_exit_check "Execute/clear_pe_matrix_port_rows"
set_directive_pipeline "Execute/execute_pe_row_jump"
set_directive_loop_flatten -off "Execute/execute_pe_col_jump"
set_directive_unroll "Execute/execute_pe_col_jump"

# Global optimisations - Step 1
# Global Consumption: 32 DSP 3911 FF 1677 LUT 9 Cycle (full cycles) 13 latency
set_directive_pipeline "multiplication_accel_top"

# Interface
# Global Consumption: 32 DSP 4380 FF 1912 LUT 9 Cycle (full cycles) 14 latency
set_directive_interface -mode axis -register -register_mode both "multiplication_accel_top" input
set_directive_interface -mode axis -register -register_mode both "multiplication_accel_top" output
set_directive_interface -mode s_axilite -register "multiplication_accel_top" min_columns
set_directive_interface -mode s_axilite -register "multiplication_accel_top" columns
set_directive_interface -mode s_axilite -register "multiplication_accel_top" min_rows
set_directive_interface -mode s_axilite -register "multiplication_accel_top" rows
set_directive_interface -mode s_axilite -register "multiplication_accel_top" max_rows
set_directive_interface -mode s_axilite -register "multiplication_accel_top" max_columns
set_directive_interface -mode s_axilite -bundle ctrl "multiplication_accel_top"
set_directive_array_partition -type complete -dim 0 "multiplication_accel_top" A
set_directive_array_partition -type complete -dim 0 "multiplication_accel_top" B

# Last set:
# Streaming continuous streaming: 32 DSP 1988 FF 1512 LUT 5 Cycle (full cycles) 3 latency