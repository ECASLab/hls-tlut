/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <kernels/arithmetic.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact GEMM implementation. It uses the conventional three-for loop
 * implementation.
 *
 * @tparam T Datatype
 * @tparam OP operators. Built-in by default
 */
template <class T, class OP = Arithmetic::Exact<T>>
struct Dense {
  const OP op{};

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
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int out = 0; out < target_outputs; ++out) {
        T val = 0.f;
        int pos = 0;
        for (int in = 0; in < input_values; ++in) {
          /* The matrix is transposed: [[w w w w],[w w w w]] weights are cols
           * and outputs are rows */
          int wpos = out * input_values + in;
          pos = in + sample * input_values;
          T weight = dense_data[wpos];
          T input = input_data[pos];
          val = op.plus(val, op.mult(weight, input));
        }
        pos = out + sample * target_outputs;
        output_data[pos] = val;
      }
    }

    return Runtime{Runtime::OK, "Dense success"};
  }
};
}  // namespace Exact

/**
 * @brief Dense wrapper for kernel executors. It validates the dense layer
 * parameters and perform the kernel launch, which is pluggable through
 * template class parameter
 *
 * @tparam T Datatype
 * @tparam K Execution kernel
 * @param ilayer layer properties and parameters
 * @param input input buffer
 * @param output output buffer
 * @return Runtime
 */
template <class T, class K = Exact::Dense<T>>
Runtime Dense(const BasicLayer *ilayer, const DataContainer *input,
              DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::Dense>;
  using QLayerType = QLayer<T, typename ::Dense>;

  /* Handle the quantisation types in a more agnostic way */
  constexpr bool kQuantised = !std::is_same<T, float>::value;
  auto qinput = dynamic_cast<const QDataContainer<T> *>(input);
  auto qoutput = dynamic_cast<QDataContainer<T> *>(output);

  Runtime ret{};

  if (!ilayer || !input || !output) {
    ret.msg = "Cannot deal with nullptr args";
    ret.code = Runtime::INVALID_PARAMETER;
    return ret;
  }

  if (kQuantised && (!qinput || !qoutput)) {
    ret.msg = "Using quantisation and setting a non-quantised data container";
    ret.code = Runtime::INVALID_PARAMETER;
    return ret;
  }

  auto layer = dynamic_cast<const LayerType *>(ilayer);
  auto qlayer = dynamic_cast<const QLayerType *>(ilayer);

  if (!layer || (kQuantised && !qlayer)) {
    ret.msg = "Layer is not compatible with Dense";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Get layer data */
  int dense_dims = layer->props.num_dimensions;
  if (dense_dims != 2) {
    ret.msg = "Dense Layer with dimensions different to 2";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }
  int dense_inputs = layer->props.dimensions[0];
  int dense_outputs = layer->props.dimensions[1];
  T *dense_data = kQuantised ? qlayer->q_data.get()
                             : reinterpret_cast<T *>(layer->data.get());

  /* Get input data - last dimension is reserved to be the number of samples */
  int input_dims = input->num_dimensions - 1;
  int input_samples = input->dimensions[input_dims];
  int input_values = 1;
  for (int i = 0; i < input_dims; ++i) {
    input_values *= input->dimensions[i];
  }
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  if (input_values != dense_inputs) {
    ret.msg =
        "Dense Layer cannot deal with a number of inputs different to the "
        "number of weights";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Compute output size */
  int target_outputs = dense_outputs;
  int target_samples = input_samples;
  int64_t target_size = target_samples * target_outputs;
  if (output->size != (target_size * sizeof(float))) {
    output->alloc = DataAllocation::HOST;

    if (kQuantised) {
      if (qoutput->GetQData(true)) delete[] qoutput->GetQData(true);
      qoutput->SetQData(new T[target_size]);
    } else {
      if (output->GetData(true)) delete[] output->GetData(true);
      output->SetData(new float[target_size]);
    }
    output->size = target_size * sizeof(float);
  }
  output->num_dimensions = 2;
  output->dimensions[0] = target_outputs;
  output->dimensions[1] = target_samples;
  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  if (input->size < (input_values * input_samples * sizeof(float))) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Input size is not completely accessible"};
  }

  if (output->size < (target_outputs * target_samples * sizeof(float))) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Output size is not completely accessible"};
  }

  /* Prepare the kernel */
  execution_kernel.target_samples = target_samples;
  execution_kernel.target_outputs = target_outputs;
  execution_kernel.input_values = input_values;

  /* Launch the kernel */
  return execution_kernel(dense_data, input_data, output_data);
}
}  // namespace Kernels
