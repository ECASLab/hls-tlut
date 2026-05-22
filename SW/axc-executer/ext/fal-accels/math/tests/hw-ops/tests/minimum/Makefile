############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# Defines the top function name
TOP_FUNCTION=minimum_accel_top
# Wilcard to access the source files
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
# Directive file
DIRECTIVE_FILE=$(abspath minimum.tcl)
# Wildcard to access the testbench files
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *-tb.hpp))
# Component name: it appears in Vivado
COMPONENT_NAME=Minimum Accelerator Example
# Name of the accelerator (same as the folder)
ACCELERATOR=minimum

# Mandatory lines for exporting the files and add the rules
export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME
include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
