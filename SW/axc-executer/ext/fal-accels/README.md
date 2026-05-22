# Flexible Accelerators Library - FAL

FAL is a set of libraries to leverage deep learning to low-end FPGAs, offering granularity at the PE level, abstracting the accelerators as a set of multiple PEs. Our accelerators aim to be adjustable in:

* Data types
* Data width and integer bit length
* Operand sizes
* Number of PEs
* Operators (approximation-friendly)

This project is a collaboration between:

* Costa Rica Institute of Technology
* University of Trieste
* Karlsruhe Institute of Technology

## Hardware accelerator generator

The building system can run co-simulations and produce the IP Cores usable in Vivado (at the moment Vivado 2018.2).

### Initialising the repository

The first step before starting:

```bash
make init
```

It will pull the `develop` versions of each dependency.

### Getting help

The project is equipped with a help target:

```bash
make help
```

### Generating documentation

You can generate the documentation by using:

```bash
export VERSION=$(cat helpers/version)
cd docs
doxygen
cd html
python3 -m http.server
```

You can find the documentation in: https://ecas-lab-tec.gitlab.io/approximate-flexible-acceleration-ml/flexible-accelerators-library-fal/

### Synthesising a design

This repo is a framework for generating new accelerators. It has equipped several examples for understanding and evaluating the code, such as GEMM, convolutions and non-linearities (a.k.a. activation functions). The following [examples](examples/) are available:

* convolution
* gemm
* addition
* activations
* minimum: a really minimum example for reference. Please, read its readme for more info.

Each accelerator is customisable. You can modify the accelerator by defining the following environment variables:

* The accelerator: `ACCELERATOR=convolution`
* The kernel size (convolution only): `Q_KS=3`
* The length of the data type (fixed-point): `Q_BW=8`
* The length of the integer part (fixed-point): `Q_INT=1`
* The output size: `Q_O=2`
* The PE core: `Q_CORE=Spatial`
* The number of PEs within the accelerator: `Q_PES=4`
* The clock period: `CLOCK_PERIOD=7.5`
* The random seed: `Q_SEED=0`
* Others...

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* Convolution:

```bash
make ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial synthesis
# if you get an error in Export RTL
ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial faketime -f  "-1y" make synthesis
```

* GEMM:

```bash
make ACCELERATOR=gemm Q_BW=10 Q_INT=1 Q_SEED=1 synthesis
# if you get an error in Export RTL
ACCELERATOR=gemm Q_BW=10 Q_INT=1 Q_SEED=1 faketime -f  "-1y" make synthesis
```

After the synthesis, you will find within the "example" a .zip with the IP core usable in Vivado and the Vivado HLS project associated with the reports. You can also find the logs in the `vivado_hls.log` file. For the example, you will find these files in: [examples/convolution/](examples/convolution/)

### Design Space Exploration

1. Requirements

This requires having MPI installed. You can install it by using:

```bash
sudo apt install openmpi-common libopenmpi2 or libopenmpi-dev
sudo apt-get install openmpi-bin
```

It is also required to have the following Python modules:

* matplotlib
* numpy
* pandas
* tabulate
* mpi4py

2. Create a folder to hold the results

The DSE will redirect the results to the created folder. In this case, assume that `results/gemm` is created:

```bash
mkdir -p results/gemm
```

3. Create/use the configuration script

This will specify the analysis and DoFs in the project. You can take the scripts in [./scripts/configs](./scripts/configs) as references. In this example, [./scripts/configs/gemm-120-simple.json](./scripts/configs/gemm-120-simple.json) will be used.

4. Execute the DSE

Execute the DSE:

```bash
mpiexec --use-hwthread-cpus -n $(proc) ./scripts/dse.py ./scripts/configs/gemm-120-simple.json --output-path results/gemm
```

### Cleaning everything

You can delete everything with:

```bash
make clean-all
```

Or a specific target by defining the modifiers and using:

```bash
make ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial clean
```

## License

* Apache v2

## Relevant links

* Official repository: [GitLab FAL](https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/flexible-accelerators-library-fal)
* Official documentation: [Documentation](https://ecas-lab-tec.gitlab.io/approximate-flexible-acceleration-ml/flexible-accelerators-library-fal/)

## About

Author:

* Luis G. Leon Vega <luis.leon@ieee.org>

Supervisor:

* Dr.-Ing. Jorge Castro Godinez <jocastro@tec.ac.cr>

Project Collaborators:

* Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
* Eduardo Salazar Villalobos <eduarsalazar@estudiantec.cr>
* Esteban Campos Granados <estebandcg1999@estudiantec.cr>
* Erick A. Obregon Fonseca <erickof@estudiantec.cr>
* Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
* David Cordero Chavarría dacoch215@estudiantec.cr

Special thanks to:

* eXact Lab S.R.L because of the PhD fellowship
* Instituto Tecnologico de Costa Rica because of the Master's scholarship
* RidgeRun LLC because of hardware facilitation
