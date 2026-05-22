############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Input Mappings
set_directive_array_partition -type complete -dim 0 "matmap_top_accel" a
set_directive_array_partition -type complete -dim 0 "matmap_top_accel" res

# Interface to the PE unit
set_directive_interface -register "MatrixMap<T, M, N, OP>::Execute" input_matrix
set_directive_interface -register "MatrixMap<T, M, N, OP>::Execute" result_matrix

# Map specific optimisations
set_directive_inline -off "MatrixMap<T, M, N, OP>::Execute"
set_directive_pipeline "MatrixMap<T, M, N, OP>::Execute"
set_directive_unroll "MatrixMap<T, M, N, OP>::Execute/rows_loop"
set_directive_unroll "MatrixMap<T, M, N, OP>::Execute/cols_loop"
