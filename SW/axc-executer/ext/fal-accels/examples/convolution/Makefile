############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

TOP_FUNCTION=convolution_accel
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
DIRECTIVE_FILE=$(abspath stream-convolution.tcl)
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *.hpp))
COMPONENT_NAME=FAL Stream Convolution
ACCELERATOR=convolution

TB_FLAGS?="-DCONV_IMAGE=$(abspath misc/lenna.png)"
CUSTOM_FLAGS+=$(TB_FLAGS)

export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME CUSTOM_FLAGS

include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
