############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# Repositories
FAL_INCLUDES += -I$(abspath $(HELPER_DIR)/../convolution/include)
FAL_INCLUDES += -I$(abspath $(HELPER_DIR)/../math/axc-math)
FAL_INCLUDES += -I$(abspath $(HELPER_DIR)/../matrix/includes)

# Other folders
FAL_INCLUDES += -I$(abspath $(HELPER_DIR)/../include)

export FAL_INCLUDES
