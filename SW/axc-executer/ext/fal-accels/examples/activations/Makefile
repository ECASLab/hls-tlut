############################################################
## Copyright 2021-2023
## Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

TOP_FUNCTION=activations_accel
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
DIRECTIVE_FILE=$(abspath stream-activations.tcl)
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *-tb.hpp))
COMPONENT_NAME=FAL Activations Accelerator
ACCELERATOR=activations

export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME

include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
