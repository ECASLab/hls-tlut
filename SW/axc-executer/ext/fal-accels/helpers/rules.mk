############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

HELPER_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(HELPER_DIR)/..
EXAMPLE_DIR := $(ROOT_DIR)/examples
EXAMPLES := $(notdir $(realpath $(dir $(wildcard $(EXAMPLE_DIR)/*/.))))

ACCELERATOR ?= convolution
VERSION := $(shell cat $(HELPER_DIR)/version)
VENDOR := Flexible Accelerator Library
LIBRARY := Deep Learning

export VERSION ACCELERATOR VENDOR LIBRARY HELPER_DIR ROOT_DIR

.phony: synthesis clean

%_:
	$(SYN_TOOL) -f ./script.tcl

.SECONDEXPANSION:
CLEAN_SUBDIRS = $$(foreach subdir,$$(ALL_SUBDIRS),$$(subdir)-clean)
BUILD_SUBDIRS = $$(foreach subdir,$$(SUBDIRS),$$(subdir)-synthesis)
CLEAN_ALL_SUBDIRS = $$(foreach subdir,$$(ALL_SUBDIRS),$$(subdir)-clean-all)

%-synthesis:
	$(Q)$(MAKE) -C $* synthesis

synthesis: $(BUILD_SUBDIRS)

%-clean:
	$(Q)$(MAKE) -C $* clean

clean: $(CLEAN_SUBDIRS)

%-clean-all:
	$(Q)$(MAKE) -C $* clean-all

clean-all: $(CLEAN_ALL_SUBDIRS)
