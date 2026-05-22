############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# Register Mappings
set_directive_array_partition -type complete -dim 0 "convolver_vector_top_accel" input
set_directive_array_partition -type complete -dim 0 "convolver_vector_top_accel" kernel
set_directive_array_partition -type complete -dim 0 "convolver_vector_top_accel" output

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
