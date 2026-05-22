############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Register Mappings
set_directive_array_partition -type complete -dim 0 "winograd_top_accel" input
set_directive_array_partition -type complete -dim 0 "winograd_top_accel" kernel
set_directive_array_partition -type complete -dim 0 "winograd_top_accel" output

# Register Hadamard optimisation
set_directive_unroll "Winograd<T, K>::Hadamard/winograd_exact_hadamard_i"
set_directive_unroll "Winograd<T, K>::Hadamard/winograd_exact_hadamard_j"

# Optimisation
set_directive_inline -off "Winograd<T, K>::Execute"
