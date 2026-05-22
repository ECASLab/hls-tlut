#!/bin/bash

#SBATCH --job-name="fal-simulation"

#SBATCH --partition=EPYC

#SBATCH --nodes=3

#SBATCH --ntasks=384

#SBATCH --ntasks-per-node=128

#SBATCH --time=2:00:00


module load architecture/AMD
module load openMPI/4.1.4/gnu/12.2.1

export PATH=/u/dssc/lleonvega/.local/bin:${PATH}

# 8 threads -> 16 processes per node
export OMP_NUM_THREADS=8
# 16 processes per node -> 80 processed in total
export NUM_PROCESSES=48

export MNIST_INPUT=/u/dssc/lleonvega/fast/mnist-input-10000.bin
export MNIST_OUTPUT=/u/dssc/lleonvega/fast/mnist-output-10000.bin
export START_PERMUTATION=`cat start.data`
export END_PERMUTATION=`cat end.data`
export JOBS_PATH=jobs.pkl

cd /u/dssc/lleonvega/fast/axc-executer/examples/benchmark/dse

mpirun -np $NUM_PROCESSES python3 main.py