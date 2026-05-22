############################################################
## Copyright 2021
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# Set your custom C-flags here
# General
Q_BW ?= 8
Q_INT ?= 1
Q_O ?= 2
Q_CORE ?= None
Q_PES ?= 4
CLOCK_PERIOD ?= 7.5

# Testbench and DSE specific
Q_SEED ?= 0
Q_PID ?= 0
Q_INPUTS_TB ?= 30
Q_OUTPUTS_TB ?= 10

# Convolution-specific
Q_KS ?= 3

# Activation-specific
Q_POINTS ?= 64
Q_MIN_NUM ?= -1
Q_MAX_NUM ?= 1
Q_MIN_DEN ?= 1
Q_MAX_DEN ?= 1
Q_ORDER ?= 2

# Approximation-specific
Q_AMETHOD ?= 0
Q_NBA ?= 0
Q_NBM ?= 0

CUSTOM_FLAGS+=-DQ_KS=$(Q_KS) -DQ_K=$(Q_KS) -DQ_BW=$(Q_BW) -DQ_O=$(Q_O) \
              -DQ_CORE=$(Q_CORE) -DQ_PES=$(Q_PES) -DQ_INT=$(Q_INT)     \
              -DQ_SEED=$(Q_SEED) -DQ_INPUTS_TB=$(Q_INPUTS_TB)          \
              -DQ_OUTPUTS_TB=$(Q_OUTPUTS_TB) -DQ_POINTS=$(Q_POINTS)    \
              -DQ_MIN_NUM=$(Q_MIN_NUM) -DQ_MAX_NUM=$(Q_MAX_NUM)        \
              -DQ_MIN_DEN=$(Q_MIN_DEN) -DQ_MAX_DEN=$(Q_MAX_DEN)        \
              -DQ_ORDER=$(Q_ORDER) -DQ_AMETHOD=$(Q_AMETHOD)              \
              -DQ_NBA=$(Q_NBA) -DQ_NBM=$(Q_NBM)

export CUSTOM_FLAGS Q_KS Q_BW Q_O Q_CORE Q_PES Q_INT CLOCK_PERIOD Q_SEED   \
              Q_PID Q_INPUTS_TB Q_OUTPUTS_TB Q_POINTS Q_MIN_NUM Q_MAX_NUM  \
              Q_MIN_DEN Q_MAX_DEN Q_ORDER Q_AMETHOD Q_NBA Q_NBM
