############################################################
## Copyright 2021
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

FOPS=$(wildcard tops/*.cpp)
POPS=$(FOPS:tops/%=%)
OPS=$(POPS:.cpp=)

define HELP_MESSAGE
------------------------------------------
-- Testing system for the HW Operations --
------------------------------------------
Usage: TEST=spatialconv make test

Environment variables:
- TEST: name of the operation. Possible values:
	{$(OPS)}
	Used in: test, measure

Targets:
- all: synthesise and runs the test for the given operation [descouraged]
- test: synthesises and runs the simulation for the given operation
- measure: synthesises, runs and extracts the logs for measurement
- measure-all: runs all the synthesis and simulations
- extract-data: cleans the logs, recompiling just the useful information

CoreOps modifiers:
- Q_KS: kernel size. i.e. Q_KS=3 for 3x3 kernels. Def: $(Q_KS)
- Q_BW: length of the integer representation (in bits). Def: $(Q_BW)
- Q_O: size of the output (in rows/cols). Def: $(Q_O)
- Q_CONV_CORE: convolver. Def: $(Q_CONV_CORE)
- Q_ACCEL: number of PEs. Def: $(Q_ACCEL)
- TB_ARGS: arguments to pass to the testbench. Def: <empty>
------------------------------------------
endef
