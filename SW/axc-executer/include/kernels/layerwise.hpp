/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <string>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <kernels/activations.hpp>
#include <kernels/arithmetic.hpp>
#include <kernels/nonlinear.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact vector implementation. It uses a complete version of the
 * addition using the system addition operator by default.
 *
 * @tparam T datatype
 * @tparam NL non-linear operator
 * @tparam AO arithmetic operator
 */
template <class T, class NL = NonLinear::STL<T>,
          class AO = axc::arithmetic::exact::Add<T>>
struct Layerwise {
  AO ao{};
  ActivationOperation<T, NL> act{};

  /** Number of elements per layer */
  int target_elements = 0;

  /** Number of layers to operate */
  int input_layers = 0;

  /** Initial value for each output */
  T initial_value = 0;

  /** Activation used at the end of the layer */
  Activations activation;

  /**
   * @brief Functor operator()
   *
   * @param input_data pointer to the input data pointers
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T **input_data, T *output_data) {
    if (!input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

    /* Extra check for the pointers */
    for (int i = 0; i < input_layers; ++i) {
      if (!input_data[i]) {
        return Runtime{
            Runtime::INVALID_PARAMETER,
            "Cannot execute kernel. One of the input operators is nullptr"};
      }
    }
#ifdef WITH_OMP
#pragma omp parallel for
#endif
    for (int elem = 0; elem < target_elements; ++elem) {
      T val = initial_value;
      for (int layer = 0; layer < input_layers; ++layer) {
        /* Operate on the layer -> output */
        val = ao.operator()(val, input_data[layer][elem]);
      }
      output_data[elem] = act(activation, val);
    }

    return Runtime{Runtime::OK, "Layerwise success"};
  }
};
}  // namespace Exact

/**
 * @brief Vector operation on layers. It performs layerwise ops with plugable
 * kernel
 *
 * @tparam T Datatype
 * @tparam K Execution kernel class
 * @param ilayer Layer properties
 * @param input Input buffers
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::ElementWise<T>>
Runtime Layerwise(const BasicLayer *ilayer, const DataContainer **input,
                  DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::Layerwise>;
  using QLayerType = QLayer<T, typename ::Layerwise>;

  /* Handle the quantisation types in a more agnostic way */
  constexpr bool kQuantised = !std::is_same<T, float>::value;
  auto qinput = reinterpret_cast<const QDataContainer<T> **>(input);
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
    ret.msg = "Layer is not compatible with Layerwise";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Check inputs and their sizes */
  for (uint i = 0; i < layer->num_inputs; ++i) {
    if (!input[i]) {
      ret.msg = "Cannot deal with nullptr args";
      ret.code = Runtime::INVALID_PARAMETER;
      return ret;
    }
  }

  uint64_t size = input[0]->size;
  for (uint i = 1; i < layer->num_inputs; ++i) {
    if (input[i]->size != size) {
      std::string msg = "Cannot deal with inputs of different sizes";
      msg = "Input 0: " + std::to_string(size) + " Input " + std::to_string(i) +
            ": " + std::to_string(input[i]->size);
      ret.msg = msg;
      ret.code = Runtime::INVALID_PARAMETER;
      return ret;
    }
  }

  /* Get input data */
  int input_dims = input[0]->num_dimensions;
  int input_values = 1;
  for (int i = 0; i < input_dims; ++i) {
    input_values *= input[0]->dimensions[i];
  }

  T **input_data = new T *[layer->num_inputs];
  for (uint i = 0; i < layer->num_inputs; i++) {
    input_data[i] = const_cast<T *>(
        kQuantised ? qinput[i]->GetQData()
                   : reinterpret_cast<const T *>(input[i]->GetData()));
  }
  auto const_input_data = const_cast<const T **>(input_data);

  /* Compute output size */
  int64_t target_size = input_values;

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

  /* Set the dimensions of the input*/
  output->num_dimensions = input_dims;
  for (int i = 0; i < input_dims; ++i) {
    output->dimensions[i] = input[0]->dimensions[i];
  }

  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  /* Prepare the kernel */
  execution_kernel.target_elements = target_size;
  execution_kernel.input_layers = layer->num_inputs;
  execution_kernel.initial_value = layer->props.initial_value;
  execution_kernel.activation = layer->activation;

  /* Launch the kernel */
  ret = execution_kernel(const_input_data, output_data);
  delete[] input_data;
  return ret;
}

}  // namespace Kernels
