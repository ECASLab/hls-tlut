############################################################
## Copyright 2021
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

define HELP_MESSAGE
-------------------------------------------------------------------------------
-- Synthesis system for the HW Operations                                    --
-------------------------------------------------------------------------------
Targets:

- init: Initialises the repositories such as matrix,
      convolution.

- synthesis: Synthesises a project. i.e. an accelerator.

- clean: Removes project folders

- clean-all: Removes all project-related files

CoreOps modifiers:

- ACCELERATOR: Chooses the accelerators. By default, the
      examples are synthesisable. Some of the available are:
      [$(EXAMPLES)]

General:
- Q_BW: Chooses the length of the word.
      Default: $(Q_BW)

- Q_INT: Chooses the integer part.
      Default: $(Q_INT)

- Q_O: Chooses the output size.
      Default: $(Q_O)

- Q_CORE: Chooses the type of core or operation (if available).
      Default: $(Q_CORE)

- Q_PES: Chooses the number of PEs of the accelerador.
      Default: $(Q_PES)

- Q_SEED: Chooses the Seed for random numbers.
      Default: $(Q_SEED)

- CLOCK_PERIOD: Chooses the clock period in [ns].
      Default: $(CLOCK_PERIOD)

- TB_ARGV: Passes arguments to the testbench.
      Default: Empty

Convolution-specific:
- Q_KS: Chooses the kernel size. Default: 3

Activation-specific:
- Q_POINTS: number of points of the LUT.
      Default: $(Q_POINTS)

- Q_MIN_NUM: minimum numerator value of the LUT input.
      Default: $(Q_MIN_NUM)

- Q_MAX_NUM: maximum numerator value of the LUT input.
      Default: $(Q_MAX_NUM)

- Q_MIN_DEN: minimum denominator value of the LUT input.
      Default: $(Q_MIN_DEN)

- Q_MAX_DEN: maximum denominator value of the LUT input.
      Default: $(Q_MAX_DEN)

- Q_ORDER: order of the Taylor approximation.
      Default: $(Q_ORDER)

Approximation-specific:
- Q_AMETHOD: approximation method (0: Exact, 1: LSBDrop, 2: LSBOr)
      Default: $(Q_AMETHOD)

- Q_NBA: number of approximated bits in the adder
      Default: $(Q_NBA)

- Q_NBM: number of approximated bits in the multiplier
      Default: $(Q_NBM)

Example:
      make ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial
-------------------------------------------------------------------------------
endef
