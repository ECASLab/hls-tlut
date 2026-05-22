/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <axc-math/non-linear.hpp>
#include <data.hpp>
#include <kernels/activations.hpp>
#include <kernels/arithmetic.hpp>
#include <kernels/nonlinear.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact passthru implementation. It uses a full pooling support
 * with multiple pooling modes. It uses the STL under the hood
 *
 * @tparam T Datatype
 * @tparam OP unary operators. Built-in by passthrough
 * @tparam NL nonlinear set of operators (activations)
 */
template <class T, class OP = axc::nonlinear::exact::PassThru<T>,
          class NL = NonLinear::STL<T>>
struct Mapper {
  OP op{};
  ActivationOperation<T, NL> act{};

  /** Number of output elements */
  int target_elements = 0;

  /** Number of input elements */
  int input_elements = 0;

  /** Minimum value allowed */
  T minimum_value = T{-1e6};

  /** Maximum value allowed */
  T maximum_value = T{1e6};

  /** Activation used at the end of the layer */
  Activations activation;

  /**
   * @brief Functor operator()
   *
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *input_data, T *output_data) {
    if (!input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

    if (target_elements != input_elements) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. Operands must match in size"};
    }

#ifdef WITH_OMP
#pragma omp parallel for
#endif
    /* Channels are treated as planes in this case */
    for (int i = 0; i < input_elements; ++i) {
      T val = op(input_data[i]);
      /* Activation */
      val = act(activation, val);
      val = val > maximum_value ? maximum_value : val;
      val = val < minimum_value ? minimum_value : val;
      output_data[i] = val;
    }

    return Runtime{Runtime::OK, "Mapper success"};
  }
};
}  // namespace Exact

/**
 * @brief Mapper wrapper. It performs the validation and invokes the
 * execution kernel for mapping
 *
 * @tparam T Datatype
 * @tparam K Execution kernel
 * @param ilayer Layer properties
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::Mapper<T>>
Runtime Mapper(const BasicLayer *ilayer, const DataContainer *input,
               DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::Mapper>;
  using QLayerType = QLayer<T, typename ::Mapper>;

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
    ret.msg = "Layer is not compatible with Mapper";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Check image dimensions */
  auto num_dims = input->num_dimensions;

  /* Compute size and mirror properties */
  output->num_dimensions = num_dims;
  int64_t output_size = 1;
  for (int i = 0; i < num_dims; ++i) {
    output_size *= input->dimensions[i];
    output->dimensions[i] = input->dimensions[i];
  }

  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  if (output->size != (output_size * sizeof(float))) {
    output->alloc = DataAllocation::HOST;

    if (kQuantised) {
      if (qoutput->GetQData(true)) delete[] qoutput->GetQData(true);
      qoutput->SetQData(new T[output_size]);
    } else {
      if (output->GetData(true)) delete[] output->GetData(true);
      output->SetData(new float[output_size]);
    }
    output->size = output_size * sizeof(float);
  }

  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  /* Prepare the kernel */
  execution_kernel.target_elements = output_size;
  execution_kernel.input_elements = output_size;
  execution_kernel.activation = layer->activation;
  execution_kernel.minimum_value = T{layer->props.minvalue};
  execution_kernel.maximum_value = T{layer->props.maxvalue};

  /* Launch the kernel */
  return execution_kernel(input_data, output_data);
}
}  // namespace Kernels
