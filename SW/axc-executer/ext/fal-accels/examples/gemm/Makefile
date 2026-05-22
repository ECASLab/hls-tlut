############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

TOP_FUNCTION=gemm_accel
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
DIRECTIVE_FILE=$(abspath stream-gemm.tcl)
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *-tb.hpp))
COMPONENT_NAME=FAL GEMM Accelerator
ACCELERATOR=gemm

export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME

include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
