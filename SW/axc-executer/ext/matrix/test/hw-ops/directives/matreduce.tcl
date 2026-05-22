############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Input Mappings
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)" a

# Interface to the PE unit
set_directive_interface -register "MatrixReduce<T, M, N, BOP>::Execute" input_matrix
set_directive_interface -register "MatrixReduce<T, M, N, BOP>::Execute" return

set_directive_pipeline "MatrixReduce<T, M, N, BOP>::Execute"
set_directive_unroll "MatrixReduce<T, M, N, BOP>::Execute/rows_reduce"
set_directive_unroll "MatrixReduce<T, M, N, BOP>::Execute/cols_reduce"
