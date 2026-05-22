############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

include $(HELPER_DIR)/coreops.mk
include $(HELPER_DIR)/repos.mk

HLS_SYN_TOOL?=vivado_hls

PARAMETERS=$(TEST)_$(Q_BW)_$(Q_INT)_$(Q_O)_$(Q_CORE)_$(Q_PES)_$(Q_PID)_$(Q_DATA_TB)_$(Q_ORDER)_$(Q_POINTS)_$(Q_MIN)_$(Q_MAX)_$(Q_NL_OP)_$(METHOD)_$(AXC_BITS)
PROJECT_NAME=hls_project_$(PARAMETERS)
ACCELERATOR_RTL=FAL_AML_$(TEST)_0.1.0.zip

export PROJECT_NAME

# IP Core High-Level Synthesis
$(ACCELERATOR_RTL): $(SRC_FILES) $(DIRECTIVE_FILE) $(TB_FILE)
	$(HLS_SYN_TOOL) -f $(HELPER_DIR)/script.tcl -l $(PROJECT_NAME).log

synthesis: $(ACCELERATOR_RTL)
	$(info Synthesis of $(TEST) complete)

clean:
	$(RM) -rf $(PROJECT_NAME) *.zip $(PROJECT_NAME).log

clean-all:
	$(RM) -rf hls_project_* *.log *.zip
