############################################################
## Copyright 2021
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

# Set your custom C-flags here
# For example:
Q_KS ?= 3
Q_BW ?= 8
Q_O ?= 2
Q_CONV_CORE ?= Spatial
Q_ACCEL ?= 4
CUSTOM_FLAGS+=-DQ_KS=$(Q_KS) -DQ_K=$(Q_KS) -DQ_BW=$(Q_BW) -DQ_O=$(Q_O) -DQ_CONV_CORE=$(Q_CONV_CORE) -DQ_ACCEL=$(Q_ACCEL)
export CUSTOM_FLAGS Q_KS Q_BW Q_O Q_CONV_CORE
