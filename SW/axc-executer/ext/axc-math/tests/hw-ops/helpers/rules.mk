############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

HELPER_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(HELPER_DIR)/..
REPO_DIR := $(ROOT_DIR)/../..
EXAMPLE_DIR := $(ROOT_DIR)/tests
EXAMPLES := $(notdir $(realpath $(dir $(wildcard $(EXAMPLE_DIR)/*/.))))

TEST ?= minimum
VERSION := $(shell cat $(ROOT_DIR)/../../version)
VENDOR := Flexible Accelerator Library (Approximate Math library)
LIBRARY := Deep Learning

# Uncomment to enable RTL Generation
#RTL_GEN :=1
#export RTL_GEN

export VERSION TEST VENDOR LIBRARY HELPER_DIR ROOT_DIR

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
