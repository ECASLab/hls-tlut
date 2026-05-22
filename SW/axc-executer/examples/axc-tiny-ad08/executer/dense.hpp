/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>
#include <kernel.hpp>
#include <layer.hpp>
#include <runtime.hpp>

#include "examples/common/kernels/arithmetic.hpp"
#include "examples/mqlenet5/executers/mqaccelerators.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

/**
 * @brief Macro to add the constructor, destructor and statics to the
 * accelerator declaration
 *
 * Use this macro to avoid code replication when inheriting from the
 * IAccelerator interface
 */
#define BEGIN_CUSTOM_ACCEL_DECLS(Q)                                     \
  static constexpr DataBits datawidth = Q::width;                       \
  static constexpr IntBits intwidth = Q::iwidth;                        \
  explicit Dense(const AccelConfig &configs) : IAccelerator{configs} {} \
  virtual ~Dense() = default;

namespace CustomAccel {
/**
 * @brief Dense Accelerator Simulator
 *
 * This simulates a dense accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration. It receives the kernel to execute
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact dense kernel.
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <class Q, class Kernel>
struct Dense : public IAccelerator {
  /* Declare constructor, destructor and others */
  BEGIN_CUSTOM_ACCEL_DECLS(Q);

  /**
   * @brief Triggers the execution of the dense engine
   *
   * Constructs the engines and launches them
   *
   * @param ilayer layer parameters
   * @param input input buffer. It can be a batch of buffers
   * @param output output buffer. It can be a batch of buffers
   * @return Runtime
   */
  Runtime Execute(const std::shared_ptr<BasicLayer> ilayer,
                  const std::shared_ptr<DataContainer> input,
                  std::shared_ptr<DataContainer> output) override {
    std::cout << "Running on Custom Accelerator of DENSE with arithmetic "
              << " Exact ( " << datawidth << " , " << intwidth << " )"
              << std::endl;
    return Kernels::Dense<Q, Kernel>(ilayer.get(), input.get(), output.get());
  }
};
}  // namespace CustomAccel

namespace CustomKernel {
/**
 * @brief Exact GEMM implementation. It uses the conventional three-for loop
 * implementation.
 *
 * @tparam T Datatype
 * @tparam OP operators. Built-in by default
 * @tparam Engine engine of the MatMul
 * @tparam SizePE size of the PE
 */
template <class T, class Engine, int SizePE = 2>
struct Dense {
  /* Engine used for computation 2x2 Matrix PE */
  Engine engine = Engine{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Length of a sample or number of outputs*/
  int target_outputs = 0;

  /** Length of an input vector */
  int input_values = 0;

  /**
   * @brief Functor operator()
   *
   * @param dense_data pointer to the weights of the dense layer
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *dense_data, const T *input_data, T *output_data) {
    if (!dense_data || !input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

#ifdef WITH_OMP
#pragma omp parallel for collapse(2)
#endif
    for (int sample = 0; sample < target_samples; sample += SizePE) {
      for (int out = 0; out < target_outputs; out += SizePE) {
        /* Output submatrix */
        T output_sm[SizePE][SizePE];
        std::fill(output_sm[0], output_sm[SizePE], T{0});

        /* Input submatrix */
        T input_sm[SizePE][SizePE];
        std::fill(input_sm[0], input_sm[SizePE], T{0});

        /* Weights submatrix */
        T weights_sm[SizePE][SizePE];
        std::fill(weights_sm[0], weights_sm[SizePE], T{0});

        /* Layer computation */
        for (int in = 0; in < input_values; in += SizePE) {
          /* Fill the input matrix */
          for (int i = 0; i < SizePE; ++i) {   /* Sample for */
            for (int j = 0; j < SizePE; ++j) { /* Input for */
              int pos = (in + j) + (sample + i) * input_values;
              input_sm[i][j] = input_data[pos];
            }
          }

          /* Fill the weight matrix
           * The matrix is transposed: [[w w w w],[w w w w]] weights are cols
           * and outputs are rows */
          for (int i = 0; i < SizePE; ++i) {   /* Output for */
            for (int j = 0; j < SizePE; ++j) { /* Weights for */
              int wpos = (out + i) * input_values + (in + j);
              weights_sm[j][i] = dense_data[wpos];
            }
          }

          /* Execute engine */
          engine.Execute(input_sm, weights_sm, output_sm, output_sm);
        }

        /* Write back the output */
        for (int i = 0; i < SizePE; ++i) {   /* Sample for */
          for (int j = 0; j < SizePE; ++j) { /* Input for */
            int pos = (out + j) + (sample + i) * target_outputs;
            output_data[pos] = output_sm[i][j];
          }
        }
      }
    }

    return Runtime{Runtime::OK, "FAL Dense success"};
  }
};
}  // namespace CustomKernel
