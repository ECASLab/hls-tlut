#!/bin/bash

set -e

# Prepare the directories
iterbuild=$1
cd ${iterbuild}

# Define parameters
CMD="./build/examples/benchmark-lenet5/benchmark-lenet5"
WEIGHTS="examples/lenet5/model-weights"
MNIST_INPUT=${MNIST_INPUT:-examples/benchmark-lenet5/model-tests/mnist-input-10000.bin}
MNIST_OUTPUT=${MNIST_INPUT:-examples/benchmark-lenet5/model-tests/mnist-output-10000.bin}
MNIST_SAMPLES=${MNIST_SAMPLES:-2000}

# Run
${CMD} ${WEIGHTS} ${MNIST_INPUT} ${MNIST_SAMPLES} ${MNIST_OUTPUT} 3 1> run.out.log 2> run.err.log

# Collect results
accuracy=$(cat run.out.log | grep "Accuracy" | awk '{print $2}')
echo -n $accuracy
