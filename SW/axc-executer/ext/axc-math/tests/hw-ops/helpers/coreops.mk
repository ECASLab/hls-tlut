############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# Set your custom C-flags here
# For example:
Q_BW ?= 8
Q_INT ?= 1
Q_O ?= 2
Q_CORE ?= None
Q_PES ?= 4
CLOCK_PERIOD ?= 7.5
Q_SEED ?= 0
Q_PID ?= 0
Q_DATA_TB ?= 30
METHOD ?= 0
AXC_BITS ?= 0

# NON-Linear Specific
Q_ORDER ?= 2
Q_POINTS ?= 64
Q_MIN ?= -16
Q_MAX ?= 16
Q_NL_OP ?= TanH

# Cutoff
Q_MAX_ERROR ?= 0.5

CUSTOM_FLAGS+=-DQ_BW=$(Q_BW) -DQ_O=$(Q_O) -DQ_CORE=$(Q_CORE) -DQ_PES=$(Q_PES) \
              -DQ_INT=$(Q_INT) -DQ_SEED=$(Q_SEED) -DQ_DATA_TB=$(Q_DATA_TB) \
              -DMETHOD=$(METHOD) -DAXC_BITS=$(AXC_BITS) -DQ_ORDER=$(Q_ORDER) \
              -DQ_POINTS=$(Q_POINTS) -DQ_MIN=$(Q_MIN) -DQ_MAX=$(Q_MAX) \
              -DQ_NL_OP=$(Q_NL_OP) -DQ_MAX_ERROR=$(Q_MAX_ERROR) -DWITH_VIVADO_HLS

export CUSTOM_FLAGS Q_BW Q_O Q_CORE Q_PES Q_INT CLOCK_PERIOD Q_SEED Q_PID     \
              Q_DATA_TB METHOD AXC_BITS Q_ORDER Q_POINTS Q_MIN Q_MAX Q_NL_OP  \
              Q_MAX_ERROR
