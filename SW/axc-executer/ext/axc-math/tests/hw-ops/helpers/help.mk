############################################################
## Copyright 2021-2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################
define HELP_MESSAGE
------------------------------------------
-- Testing system for the HW Operations --
------------------------------------------
Usage: TEST=exact make test

Environment variables:
- TEST: name of the operation. Possible values:
	{$(EXAMPLES)}
	Used in: synthesis

Targets:
- all: synthesise and runs the test for the given operation [descouraged]
- synthesis: synthesises and runs the simulation for the given operation

CoreOps modifiers:
- Q_BW: word length of datatype. Def: $(Q_BW) 
- Q_INT: integer length of datatype. Def: $(Q_INT)
- Q_O: size of the output of the operator. Def: $(Q_O)
- Q_CORE: operator implementation to choose. Def: $(Q_CORE)
- Q_PES: number of units to vectorise. Def: $(Q_PES)
- CLOCK_PERIOD: clock period to simulate and implement. Def: $(CLOCK_PERIOD)
- Q_SEED: seed uses for generating random numbers. Def: $(Q_SEED)
- Q_PID: PID of the process running the synthesis. Def: $(Q_PID)
- Q_DATA_TB: number of data to test with. Def: $(Q_DATA_TB)
- METHOD: method of approximation (EXACT=0, LSBDROP=1, LSBOR=2, LUT=1, TAYLOR=2, EXP_BASED=3). Def: $(METHOD)
- AXC_BITS: number of approximate bits. Def: $(AXC_BITS)
- Q_ORDER: order of the Taylor approximation. Def: $(Q_ORDER)
- Q_POINTS: number of points of the LUT. Def: $(Q_POINTS)
- Q_MIN: minimum value of the LUT input. Def: $(Q_MIN)
- Q_MAX: minimum value of the LUT input. Def: $(Q_MAX)
- Q_NL_OP: non-linear operation to test (TanH, Exponential). Def: $(Q_NL_OP)
------------------------------------------
endef
