############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

include helpers/coreops.mk
include helpers/help.mk
include helpers/rules.mk
include helpers/welcome.mk

# --- Top rules ---
.PHONY: init synthesis clean help

SUBDIRS = examples
ALL_SUBDIRS = $(SUBDIRS)

init:
	$(info $(WELCOME_MESSAGE))
	$(info Initialising submodules)
	git submodule update --init --recursive

synthesis:
	$(info $(WELCOME_MESSAGE))
	$(info Synthesising modules: $(ACCELERATOR))

help:
	$(info $(WELCOME_MESSAGE))
	$(info $(HELP_MESSAGE))

clean:
	$(info $(WELCOME_MESSAGE))
	$(info Cleaning projects)

clean-all:
	$(info $(WELCOME_MESSAGE))
	$(info Cleaning projects)
