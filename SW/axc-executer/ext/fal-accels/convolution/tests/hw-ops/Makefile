############################################################
## Copyright 2021
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

include helpers/coreops.mk
include helpers/help.mk

SYN_TOOL?=vivado_hls

# Define environment variables for the script
PARAMETERS=$(TEST)_$(Q_KS)_$(Q_BW)_$(Q_O)_$(Q_CONV_CORE)
PROJECT_NAME=hls_project_$(PARAMETERS)
TOP_FUNCTION=$(TEST)_top_accel
SRC_FILES=$(wildcard tops/$(TEST).cpp) $(wildcard tops/$(TEST)_core.cpp) $(wildcard tops/$(TEST).hpp)
TB_FILE=testbenches/$(TEST)_tb.cpp
DIRECTIVE_FILE=directives/$(TEST).tcl

# Files for the reports
REPORT_FILE=measurements/$(PARAMETERS).xml
SIMULATION_FILE=measurements/$(PARAMETERS).log

SYNTHESIS_REPORT=$(PROJECT_NAME)/solution/syn/report/$(TOP_FUNCTION)_csynth.xml
SIMULATION_LOG=$(PROJECT_NAME)/solution/csim/report/$(TOP_FUNCTION)_csim.log

export PROJECT_NAME TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE

# --- Top rules ---
.PHONY: test clean help

# Test is the variable where the op is specified
test: $(TEST)_
	$(info "Constructing and testing $(TEST)")

# Get measurements
measure: $(REPORT_FILE) $(SIMULATION_FILE)
	$(info "Measurements taken. Please, find them in measurements/")

# Massive generation - Do not run in parallel
measure-all:
	mpirun bash helpers/measure-all.sh

clean-data:
	$(RM) measurements/processed/*.data measurements/processed/*.svg

# Extracts the errors and other useful data
extract-data: clean-data
	$(RM) measurements/processed/*.data measurements/processed/*.svg
	python3 helpers/extract-report.py
	bash helpers/extract-errors.sh

help:
	$(info $(HELP_MESSAGE))

clean:
	$(RM) -r hls_project_* *.log

# --- Auxiliar ---
# Run synthesis
%_:
	$(SYN_TOOL) -f script.tcl

# Run synthesis for the report
$(SYNTHESIS_REPORT): $(TEST)_
	$(info "Simulation ran")

$(SIMULATION_LOG): $(TEST)_
	$(info "Simulation ran")

# Copy files to finish the measurement
$(REPORT_FILE): $(SYNTHESIS_REPORT)
	cp -f $< $@

$(SIMULATION_FILE): $(SIMULATION_LOG)
	cp --backup=numbered $< $@

