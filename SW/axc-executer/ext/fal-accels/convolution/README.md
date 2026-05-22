# Approximate Convolution Accelerator

## Software version

### Compiling

For compiling the code, no-dependencies are needed so far. You may only need the `meson` building system. Please, follow these steps to get it into your system:

```bash
sudo apt update
sudo apt install python3 python3-pip ninja-build
sudo pip3 install meson
sudo apt install openmpi-common libopenmpi2
pip3 install matplotlib numpy
```

After installing the building system, compile the code:

```bash
meson builddir
ninja -C builddir
```

### Testing

You can test the code by using:

```bash
cd builddir/tests/sw-ops
./convolution # For spatial convolution
```

## Hardware version

For the hardware version, you will need Vivado HLS 2018.2.

You can run the tests in the following way:

```bash
# Run a synthesis + simulation
cd tests/hw-ops/
TEST=spatialconv make test

# For synthesis + simulation where the testbench has arguments - recommended
export TB_ARGV="`pwd`/misc/lenna.png `pwd`/misc/output.png" 
Q_KS=3 Q_ACCEL=4 Q_CONV_CORE=Spatial TEST=accel_stream make test

# Run everything - with a Kernel Size of 3x3
export MIN_Q_K=3
export MAX_Q_K=3
export TB_ARGV="`pwd`/misc/lenna.png `pwd`/misc/output.png" 
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

* TEST: name of the tests. Please, inspect `tests/hw-ops/tops/*.cpp` for the possible tests. You can also use `make help`.
* Q_KS: kernel size. i.e. Q_KS=3 for 3x3 kernels. Def: 3
* Q_BW: length of the integer representation (in bits). Def: 8
* Q_O: size of the output (in rows/cols). Def: 2
* Q_CONV_CORE: convolver [Winograd, Spatial]. Def: Spatial
* Q_ACCEL: number of PEs. Def: 4
* TB_ARGS: arguments to pass to the testbench. Def: <empty>
* SYN_TOOL: name of the synthesis tool. By default, it is `vivado_hls`

> If something fails and it is related to `vivado_hls not found`, please, make sure of having it in the environment. Usually, it requires:
> `source /opt/Xilinx/Vivado/2018.2/settings64.sh`

## How to add a new hardware testbench

1. Create a directive file: you can simply copy and paste one of the existing in `directives/`. The syntax is: `${TEST}.tcl`.
2. Create the top files: they are in `tops/`. The filenames must match the following syntax: `${TEST}.cpp` and `${TEST}.hpp`.
3. The top function must follow the syntax: `${TEST}_top_accel`.
4. Create a testbench. The testbench file must be named as `${TEST}_tb.cpp`.
5. Add the test to `helpers/measure-all.sh` to `ACCELS`. For example:
`ACCELS=${ACCELS:-"spatialconv winograd"}`, where `winograd` is a new test.

## Extracting data and generating plots

The project includes a testbench capable of benchmarking several convolution implementations as a part of the design exploration. To generate the data, run:

```bash
make measure-all
make extract-data
```

## Important data

After the extraction, the relevant files are:

```
test/hw-ops/measurements/processed
  |_ plot_hist_datatypes_${ACCEL}_${IMG}_${SIZE}.svg -> Report of the error distribution
  |_ plot_${METRIC}_${ACCEL}.svg                     -> Report of the error metrics
  |_ hist_*                                          -> Histogram data
  |_ stats_*                                         -> Statistics and metrics data
```

## Known issues

### PNG and JPEG issues

'''Issue:'''

```
./Vivado/2018.2/lnx64/tools/opencv/opencv_gcc/libopencv_highgui.so: undefined reference to `png_read_info@PNG12_0'
./Vivado/2018.2/lnx64/tools/opencv/opencv_gcc/libopencv_highgui.so: undefined reference to `jpeg_finish_compress@LIBJPEG_6.2'
./Vivado/2018.2/lnx64/tools/opencv/opencv_gcc/libopencv_highgui.so: undefined reference to `jpeg_write_scanlines@LIBJPEG_6.2'
./Vivado/2018.2/lnx64/tools/opencv/opencv_gcc/libopencv_highgui.so: undefined reference to `png_set_packing@PNG12_0'
```

'''Fix:'''

Install the libraries:

```bash
wget http://se.archive.ubuntu.com/ubuntu/pool/main/libp/libpng/libpng12-0_1.2.54-1ubuntu1_amd64.deb
sudo apt install ./libpng12-0_1.2.54-1ubuntu1_amd64.deb
sudo apt-get install libjpeg62
```
```bash
./open ~/Downloads/Lenna.png
```
Author:

* Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
* Luis G. Leon Vega <lleon95@estudiantec.cr>
