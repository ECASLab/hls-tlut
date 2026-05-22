############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

set_directive_pipeline "$::env(TOP_FUNCTION)"
set_directive_inline "MatrixMultiply<T, M, N, ADD, MULT, NL>::Execute"

set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)" a
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)" b
set_directive_array_partition -type complete -dim 0 "$::env(TOP_FUNCTION)" res
