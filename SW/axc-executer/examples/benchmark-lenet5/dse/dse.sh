#!/bin/bash

export OMP_NUM_THREADS=4
export NUM_PROCESSES=2

export MNIST_INPUT=${MNIST_INPUT:-./examples/benchmark-lenet5/model-tests/mnist-input-10000.bin}
export MNIST_OUTPUT=${MNIST_INPUT:-./examples/benchmark-lenet5/model-tests/mnist-output-10000.bin}
export MNIST_SAMPLES=${MNIST_SAMPLES:-5000}

mpirun -np ${NUM_PROCESSES} python3 main.py