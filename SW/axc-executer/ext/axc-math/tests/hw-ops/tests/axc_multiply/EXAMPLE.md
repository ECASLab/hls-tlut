# Multiply example

This example is aimed to provide an example of how to integrate a multiply HLS project for experimentation. This is intended to work as an example for synthesisable IP Cores for Xilinx/AMD platforms, with co-simulation.

## Steps to create a multiply accelerator

1. Define a [Makefile](./Makefile):

```makefile
# Defines the top function name
TOP_FUNCTION=multiply
# Wilcard to access the source files
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
# Directive file
DIRECTIVE_FILE=$(abspath multiply.tcl)
# Wildcard to access the testbench files
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *-tb.hpp))
# Component name: it appears in Vivado
COMPONENT_NAME=Multiply Accelerator Example
# Name of the accelerator (same as the folder)
ACCELERATOR=multiply

# Mandatory lines for exporting the files and add the rules
export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME
include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
```

2. Define a [directive file](./multiply.tcl):

```t
# Add the following line to define the top function dynamically
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# You can add more directives below
```

3. Add your source code:

We recommend to follow this convention:

* `multiply.hpp`, `multiply.cpp` for functionality and logic
* `multiply-core.cpp` for the top function and accelerator wrapper
* `multiply-tb.hpp`, `multiply-tb.cpp` for the testbench files

4. Run the synthesis:

When everything has been completed, you can generate the IP core and run
the simulations with:

```bash
ACCELERATOR=multiply make
```

It will generate a folder called `hls_project_multiply_*` with the contents
of the Vivado HLS project. Also, when it finishes, it will pack the IP core
in a zip file (`FAL_multiply_*-0.1.0`).
