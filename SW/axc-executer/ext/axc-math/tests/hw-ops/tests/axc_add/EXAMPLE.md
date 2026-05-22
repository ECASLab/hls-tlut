# Add example

This example is aimed to provide an example of how to integrate a add HLS project for experimentation. This is intended to work as an example for synthesisable IP Cores for Xilinx/AMD platforms, with co-simulation.

## Steps to create a add accelerator

1. Define a [Makefile](./Makefile):

```makefile
# Defines the top function name
TOP_FUNCTION=add_accel_top
# Wilcard to access the source files
SRC_FILES=$(abspath $(wildcard *-hw.cpp) $(wildcard *-core.cpp) $(wildcard *.hpp))
# Directive file
DIRECTIVE_FILE=$(abspath add.tcl)
# Wildcard to access the testbench files
TB_FILE=$(abspath $(wildcard *-tb.cpp) $(wildcard *-tb.hpp))
# Component name: it appears in Vivado
COMPONENT_NAME=Add Accelerator Example
# Name of the accelerator (same as the folder)
ACCELERATOR=add

# Mandatory lines for exporting the files and add the rules
export TOP_FUNCTION SRC_FILES TB_FILE DIRECTIVE_FILE COMPONENT_NAME
include ../../helpers/rules.mk
include ../../helpers/synthesis.mk
```

2. Define a [directive file](./add.tcl):

```t
# Add the following line to define the top function dynamically
set_directive_top -name $::env(TOP_FUNCTION) "$::env(TOP_FUNCTION)"

# You can add more directives below
```

3. Add your source code:

We recommend to follow this convention:

* `add.hpp`, `add.cpp` for functionality and logic
* `add-core.cpp` for the top function and accelerator wrapper
* `add-tb.hpp`, `add-tb.cpp` for the testbench files

4. Run the synthesis:

When everything has been completed, you can generate the IP core and run
the simulations with:

```bash
TEST=add make
```

It will generate a folder called `hls_project_add_*` with the contents
of the Vivado HLS project. Also, when it finishes, it will pack the IP core
in a zip file (`FAL_add_*-0.1.0`).
