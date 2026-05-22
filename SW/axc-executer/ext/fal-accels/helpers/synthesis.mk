############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

include $(HELPER_DIR)/coreops.mk
include $(HELPER_DIR)/repos.mk

HLS_SYN_TOOL?=vivado_hls

PARAMETERS=$(ACCELERATOR)_$(Q_KS)_$(Q_BW)_$(Q_INT)_$(Q_O)_$(Q_CORE)_$(Q_PES)_$(Q_INPUTS_TB)_$(Q_OUTPUTS_TB)_$(Q_SEED)_$(Q_POINTS)_$(Q_MIN_NUM)_$(Q_MAX_NUM)_$(Q_MIN_DEN)_$(Q_MAX_DEN)_$(Q_ORDER)_$(Q_AMETHOD)_$(Q_NBA)_$(Q_NBM)

PROJECT_NAME=hls_project_$(PARAMETERS)
ACCELERATOR_RTL=FAL_$(ACCELERATOR)_0.1.0.zip

export PROJECT_NAME

# IP Core High-Level Synthesis
$(ACCELERATOR_RTL): $(SRC_FILES) $(DIRECTIVE_FILE) $(TB_FILE)
	$(HLS_SYN_TOOL) -f $(HELPER_DIR)/script.tcl -l $(PROJECT_NAME).log

synthesis: $(ACCELERATOR_RTL)
	$(info Synthesis of $(ACCELERATOR) complete)

clean:
	$(RM) -rf $(PROJECT_NAME) *.zip $(PROJECT_NAME).log

clean-all:
	$(RM) -rf hls_project_* *.log *.zip
