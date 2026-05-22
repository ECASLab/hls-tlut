# Approximate Matrix Accelerator

This repo presents:

* A templated library for the automatic generation and specialisation of Matrix operations for High-Level Synthesis (with Vivado HLS). The Matrix operations can be specialised in:
  * Matrix size
  * Data type
  * Operands
* A comprehensive evaluation of the performance, design efficiency, and error. More information in [report](test/hw-ops/measurements/processed)


The design efficiency is a home-made figure of merit for comparing the PE design in terms of: *performance, number of operations, and resource occupation*.

To use the library, you can install the include in your project and use:

```c++
#include "linear.hpp"
```

Some examples are in [tests](test/hw-ops).

The documentation is available [here](https://ecas-lab-tec.gitlab.io/approximate-flexible-acceleration-ml/approximate-gemm-accelerator).

The latest source code is available in our [GitLab](https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/approximate-gemm-accelerator)

## Software version

### Compiling

For compiling the code, no dependencies are needed so far. You may only need the `meson` building system. Please, follow these steps to get it into your system:

```bash
sudo apt update
sudo apt install python3 python3-pip ninja-build
sudo pip3 install meson
sudo apt install openmpi-common libopenmpi2 or libopenmpi-dev
sudo apt-get install openmpi-bin
pip3 install matplotlib numpy
```
#### Note for Ubuntu 16.04

We have found some difficulties while installing Python, Meson and Ninja on Ubuntu 16.04. Please, follow these steps to deal with the versioning issues.

1. For Python3 and Pip3

Installing miniconda:

```bash
wget https://repo.anaconda.com/miniconda/Miniconda3-py38_4.10.3-Linux-x86_64.sh
bash Miniconda3-py38_4.10.3-Linux-x86_64.sh
```

During the installation, it will ask for installation details. Just accept the defaults. At the end, your console will reload with a conda environment. You should be able to install meson without any issues by using `pip3`.

2. For Ninja

The `apt` repos are really out-dated. Install ninja by doing:

```bash
wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
sudo unzip ninja-linux.zip -d /usr/local/bin/
```

From now on, `ninja` has been installed.

After installing the building system, compile the code:

```bash
meson builddir
ninja -C builddir
```

### Testing

You can test the code by using:

```bash
cd builddir/test/sw-ops
./matadd # For addition
./matmul # For multiplication
./matmac # For multiply-add
./matfma # For fused multiply-add
./matstrassen # For matrix multiplication using the Strassen algorithm
./matwinograd # For matrix multiplication using the Winograd-Strassen algorithm
```

or

```bash
ninja -C builddir test
```

> Deprecation warning. In next releases, the SW version will be deprecated.

## Hardware version

For the hardware version, you will need Vivado HLS 2018.2.

You can run the tests in the following way:

```bash
# Run a synthesis + simulation
cd test/hw-ops/
TEST=matadd make test

# Run everything
make measure-all

# Extracts data
make extract-data

# Clean all
make clean
```

where the possible targets are:

* test: performs the tests on a given testbench
* measure: synthesises, runs and extracts the logs for measurement
* measure-all: runs all the synthesis and simulations
* extract-data: cleans the logs, recompiling just the useful information

and where the environment variables are:

* TEST: name of the tests. Please, inspect `test/hw-ops/tops/*.cpp` for the possible tests. You can also use `make help`.
* Q_ROWS: Number of rows. Default: 8
* Q_COLS: Number of columns. Default: 8
* Q_SEED: Seed for generating random numbers. Default: 10
* Q_DATATYPE: datatype of hardware matrices. Default: ap_fixed
* Q_WL: word length of datatype. Default: 8
* Q_USE_HLS_MATH: enables the hls math library. Default: 1
* Q_USE_REG_UNROLLING: selects which scenario use in matfma operation. Default: 0, for scenario 1.
* SYN_TOOL: name of the synthesis tool. By default, it is `vivado_hls`

> If something fails and it is related to `vivado_hls not found`, please, make sure of having it in the environment. Usually, it requires:
> `source /opt/Xilinx/Vivado/2018.2/settings64.sh`

## Important data

After the extraction, the relevant files are in [report](test/hw-ops/measurements/processed)

Documentation: available [here](https://ecas-lab-tec.gitlab.io/approximate-flexible-acceleration-ml/approximate-gemm-accelerator).


## How to cite

Please, feel free to cite any of the following sources:

### Plain Text

* L. G. León-Vega, E. Salazar-Villalobos and J. Castro-Godínez, "An Exploration of Accuracy Configurable Matrix Multiply-Addition Architectures using HLS," 2022 IEEE 15th Dallas Circuit And System Conference (DCAS), Dallas, TX, USA, 2022, pp. 1-6, doi: 10.1109/DCAS53974.2022.9845501.
* Luis G. León-Vega, Eduardo Salazar-Villalobos, Alejandro Rodriguez-Figueroa, and Jorge Castro-Godínez. 2023. Automatic Generation of Resource and Accuracy Configurable Processing Elements. ACM Trans. Embed. Comput. Syst. 22, 4, Article 75 (July 2023), 27 pages. https://doi.org/10.1145/3594540
* Salazar-Villalobos, Eduardo, Leon-Vega, Luis G., & Castro-Godinez, Jorge. (2023). Flexible Accelerator Library: Approximate Matrix Accelerator (v1.3.0). Zenodo. https://doi.org/10.5281/zenodo.10017893

### BibTeX

```bibtex
@INPROCEEDINGS{dcas-leon-vega,
   AUTHOR     = {León-vega, Luis G.
               AND Salazar-villalobos, Eduardo
               AND Castro-godínez, Jorge},
   BOOKTITLE  = {2022 IEEE 15th Dallas Circuit And System Conference (DCAS)}, 
   TITLE      = {An Exploration of Accuracy Configurable Matrix Multiply-Addition Architectures using HLS}, 
   YEAR       = {2022},
   VOLUME     = {},
   NUMBER     = {},
   PAGES      = {1-6},
   DOI        = {10.1109/DCAS53974.2022.9845501}}
@article{tecs-leon-vega-2023,
   AUTHOR     = {Le\'{o}n-vega, Luis G.
               AND Salazar-villalobos, Eduardo
               AND Rodriguez-figueroa, Alejandro
               AND Castro-god\'{\i}nez, Jorge},
   TITLE      = {{Automatic Generation of Resource and Accuracy Configurable Processing Elements}},
   YEAR       = {2023},
   PUBLISHER  = {Association for Computing Machinery},
   ADDRESS    = {New York, NY, USA},
   ISSN       = {1539-9087},
   URL        = {https://doi.org/10.1145/3594540},
   DOI        = {10.1145/3594540},
   JOURNAL    = {ACM Trans. Embed. Comput. Syst.},
   MONTH      = {apr},
   KEYWORDS   = {Approximate computing, Deep neural networks, Hardware acceleration, Neural network hardware, Edge computing, Multi-layer neural network}
}
```

## Acknowledgements:

This work was possible thanks to the Programmi Operativi Nazionali (PON) from Ministero dell'Università e della Ricerca, eXact Lab S.R.L, the postgraduate research scholarship from Instituto Tecnológico de Costa Rica, and the Master's scholarship programme from RidgeRun Embedded Solutions LLC.


Authors:

* Eduardo Salazar Villalobos <eduarsalazar@estudiantec.cr>
* Alex Chacon-Rodriguez <alexchr10@estudiantec.cr>

Supervisor:

* Luis G. Leon Vega <lleon95@estudiantec.cr>, <luis.leon@ieee.org>
* Jorge Castro-Godinez <jocastro@tec.ac.cr>
