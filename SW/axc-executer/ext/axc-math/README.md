# Approximate Math Operators

## Software version

### Compiling

For compiling the code, no-dependencies are needed so far. You may only need the `meson` building system. Please, follow these steps to get it into your system:

```bash
sudo apt update
sudo apt install python3 python3-pip ninja-build
sudo pip3 install meson
```

Then, initialise the subrepos:

```bash
git submodule update --init --recursive
```

Then, compile the code:

```bash
meson builddir --prefix=$(pwd)/project
ninja -C builddir
```

> For the building options, consider using `meson configure`

### Testing

You can test the code by using:

```bash
meson builddir -Dbuild_tests=true
ninja -C builddir test
```

### Installation

Please, do not install the code into your machine given that it is not intended to be software.

You can reconfigure the code into a different project path:

```bash
meson --reconfigure builddir --prefix=$(pwd)/project
```

where `project` is the folder within the root of the repo.

Then, install:

```bash
ninja -C builddir install
```

It will install into: `project/` with the following tree:

```
project/
└── include
    └── axc-math
        ├── arithmetic.hpp
        ├── exact
        │   ├── *.hpp
        ├── non-linear.hpp
        └── operators.hpp
```

### Documentation

To generate the documentation, please, do:

```bash
cd docs
doxygen
```

### Contributing

Please, make sure that your code is properly indented and following the Google Style for C++. You can indent your code with:

```bash
clang-format -i --style=google ${FILE}
```

## Hardware version

### Requisites:

* Vivado HLS 2018.2

### Hardware accelerator generator

The building system can run simulations and produce the RTL usable in Vivado.

#### Getting help

The project is equipped with a help target:

```bash
make help
```

#### Generating documentation

T.B.D

### Synthesising a design

This repo is a framework for testing implementations. It has equipped some examples for understanding and evaluating the code, such as minimum, exact and others. The following [examples](tests/hw-ops/tests) are available:

* minimum

Each accelerator is customisable. You can modify the accelerator by defining the following environment variables:

* `Q_BW`: word length of datatype. Def: 8 
* `Q_INT`: integer length of datatype. Def: 1
* `Q_O`: size of the output of the operator. Def: 2
* `Q_CORE`: operator implementation to choose. Def: None
* `Q_PES`: number of units to vectorise. Def: 4
* `CLOCK_PERIOD`: clock period to simulate and implement. Def: 7.5
* `Q_SEED`: seed uses for generating random numbers. Def: 0
* `Q_PID`: PID of the process running the synthesis. Def: 0
* `Q_DATA_TB`: number of data to test with. Def: 30
* Others ...

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* Minimum:

```bash
make TEST=minimum Q_BW=10 Q_INT=2 Q_CORE=exact synthesis
```

After the synthesis, you will find the logs in the `hls_project_*.log` file and the HLS project in the `hls_project_*` folder. For the example, you will find these files in: [tests/hw-ops/tests/minimum](tests/hw-ops/tests/minimum)

### Design Space Exploration

T.B.D

### Cleaning everything

You can delete everything with:

```bash
make clean-all
```

Or a specific target by defining the modifiers and using:

```bash
make TEST=minimum Q_BW=10 Q_INT=2 Q_CORE=exact clean
```

## Missing features:

* Log collection
* Part replacement
* Design Exploration Analysis
* RTL generation

-----

## About

Author:

* Luis G. Leon Vega <lleon95@estudiantec.cr>

Supervisor:

* Dr.-Ing. Jorge Castro Godinez <jocastro@tec.ac.cr>

Project Collaborators:

* Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
* David Cordero Chavarría <dacoch215@estudiantec.cr>

Special thanks to:

* eXact Lab S.R.L because of the PhD fellowship
* Instituto Tecnologico de Costa Rica because of the Master's scholarship
* RidgeRun LLC because of hardware facilitation
