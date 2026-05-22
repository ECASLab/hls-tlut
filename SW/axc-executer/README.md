# AxC Executer

This repo contains a library to perform software simulations of accelerators for the FAL library.

## Requirements

* C++17
* Meson
* Ninja Build
* OpenMP: optional

# Preparation

```bash
git submodule update --init --recursive
```

## Compilation

```bash
meson builddir
ninja -C builddir
```

You can tune the build with the following options:

Project options   | Default Value     | Possible Values                    | Description
------------------|-------------------|------------------------------------|-----------------------------------
arithmetic-mode   | EXACT             | [EXACT, LSBDROP, LSBOR, LSBFIXED]                             | Approximation mode of the arithmetic kernels
axc-drop          | 2                 | >=1, <=8                                                      | Number of bits to drop in AxC Math
axc-external      | 20                | >=14, <=24                                                    | Number of bits for external layers
axc-integer       | 6                 | >=1, <=8                                                      | Number of integer bits for Fixed-Point
axc-internal      | 16                | >=10, <=20                                                    | Number of bits for internal layers
axc-width         | 14                | >=4, <=16                                                     | Number of bits for Fixed-Point
debug-mode        | false             | [true, false]                                                 | Enable debugging mode
developer-mode    | true              | [true, false]                                                 | Enable developer mode
matrix-multiplier | MatrixMultiplyAdd | [MatrixMultiplyAdd, MatrixMultiplyAddStrassen, MatrixMultiplyAddWinograd]     | Matrix Multiply Core
accelerators      | None              | [None, Emulated]     | Enables the accelerator logic where it is possible


## Execution

> All the inputs/outputs are in float32

There are some examples that illustrate the usage of the library. For instance:

* Simple LeNet5 with FP32:

```bash
./builddir/examples/lenet5/lenet5 examples/lenet5/model-weights/ examples/lenet5/model-tests/test-7.bin 1
```

where the arguments are:

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES_IN_INPUT
```

They perform an inference of the number 7 on a pre-trained model of the LeNet5 on the MNIST dataset

* LeNet5 with quantisation

```bash
./builddir/examples/qlenet5/qlenet5 examples/lenet5/model-weights/ examples/lenet5/model-tests/test-7.bin 1
```

> the quantisation parameters are `axc-integer` and `axc-width`

where the arguments are:

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES_IN_INPUT
```

They perform an inference of the number 7 on a pre-trained model of the LeNet5 on the MNIST dataset

* LeNet5 with approximate math

```bash
./builddir/examples/axc-qlenet5/axc-qlenet5 examples/lenet5/model-weights/ examples/lenet5/model-tests/test-7.bin 1
```

where the arguments are:

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES_IN_INPUT
```

> the quantisation parameters are `arithmetic-mode`, `axc-drop`, `axc-integer` and `axc-width`

They perform an inference of the number 7 on a pre-trained model of the LeNet5 on the MNIST dataset

* AD08 with multi-quantisation and approxmation

```bash
./builddir/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/input_inference_196_64.bin 1 out.bin
```

where the arguments are:

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES_IN_INPUT OUTPUT_FILE
```

> the quantisation parameters are `arithmetic-mode`, `axc-drop`, `axc-integer` and `axc-width`

* Image Processing: Warp Perspective

```bash
./builddir/examples/warp-perspective/warp-perspective examples/warp-perspective/misc/peppers.png
```

where the argument is an image.

> this does not have any tuning parameter. This is a baseline for future research. It has pending the evaluation of SSIM and PSNR. Moreover, the introduction of approximate logic and variable numerical precision is still missing.

It performs a Warp Perspective and uses a dense multiplication for computing the output mapping.

* Signal Processing: Audio FFT

```bash
./builddir/examples/fft-1D/fft-1D examples/fft-1D/misc/sample_in_1024.bin fft-out.bin examples/fft-1D/misc/w_cos_1024.bin examples/fft-1D/misc/w_sin_1024.bin
```

where the arguments are:

```
input.bin output.bin w_cos.bin w_sin.bin
```

* input.bin: input binary with 1024 samples in float32
* output.bin: output binary with 512 samples in float32
* w_cos.bin: matrix of 512x1024 with cosine values for DFT
* w_sin.bin: matrix of 512x1024 with sine values for DFT

> this does not have any tuning parameter. This is a baseline for future research. It has pending the evaluation of PSNR. Moreover, the introduction of approximate logic and variable numerical precision is still missing.

It performs a 1024 FFT on an audio signal to proceed with the Mel Spectrogram utilised in the AD08 model

* MobileNetV2 with FP32:

```bash
NUM_SAMPLES=50

./builddir/examples/axc-mobilenetv2/axc-mobilenetv2 examples/axc-mobilenetv2/weights examples/axc-mobilenetv2/test/inputs-50.bin ${NUM_SAMPLES} examples/axc-mobilenetv2/test/outputs-50.bin
```

where the arguments are:

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES_IN_INPUT OUTPUT_REFERENCE
```

This example performs an image classification on a reduced ImageNet dataset (ImageNette): https://github.com/fastai/imagenette. The FP32 accuracy is 0.846 in the 500 samples and 0.87 in the 50 samples

## Benchmarking

To run a whole analysis for greedy DSE

```bash
./builddir/examples/benchmark-lenet5/benchmark-lenet5 examples/lenet5/model-weights examples/benchmark-lenet5/model-tests/mnist-input-10000.bin 10 examples/benchmark-lenet5/model-tests/mnist-output-10000.bin 1
```

where the parameters are

```
PATH_TO_WEIGHTS INPUT_TEST SAMPLES EXPECTED_TEST RUN
```

where RUN is 

* 0 for exact
* 1 for quantisation
* 2 for approximation
* 3 for multi-quantisation + approximation

## Repo information:

* This repo: https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/axc-executer
* Axc-Math: https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/approximate-math-operators 

## Authors

* David Cordero Chavarría <dcorderoch@ieee.org>: student
* Luis G. Leon Vega <luis.leon@ieee.org>: supervisor
