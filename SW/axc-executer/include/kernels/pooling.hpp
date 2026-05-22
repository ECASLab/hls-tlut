/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact pooling implementation. It uses a full pooling support
 * with multiple pooling modes. It uses the STL under the hood
 *
 * @tparam T Datatype
 * @tparam OP operators. Built-in by default
 */
template <class T, class OP = Arithmetic::Exact<T>>
struct Pooling {
  const OP op{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of channels included in an output sample */
  int target_channels = 0;

  /** Width of a sample. It must match the input */
  int target_width = 0;

  /** Height of a sample. It must match the input */
  int target_height = 0;

  /** Number of channels included in an input sample */
  int input_channels = 0;

  /** Width of a sample. It must match the output */
  int input_width = 0;

  /** Height of a sample. It must match the output */
  int input_height = 0;

  /** Pooling type */
  Layers type;

  /** Pooling stride in X */
  int stride_x = 0;

  /** Pooling stride in Y */
  int stride_y = 0;

  /** Pooling size in X */
  int pooling_y = 0;

  /** Pooling size in Y */
  int pooling_x = 0;

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

#ifdef WITH_OMP
#pragma omp parallel for collapse(2)
#endif
    /* Channels are treated as planes in this case */
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int map = 0; map < target_channels; ++map) {
        for (int y = 0; y < target_height; ++y) {
          for (int x = 0; x < target_width; ++x) {
            T val = 0;
            int xi = x * stride_x;
            int yi = y * stride_y;
            int pos = 0;

            std::vector<T> pool_elems{};

            for (int j = 0; j < pooling_y; ++j) {
              for (int i = 0; i < pooling_x; ++i) {
                pos = (xi + i) + (yi + j) * input_width;
                pos += input_height * input_height * map;
                pos += input_height * input_height * input_channels * sample;

                T elem = *(input_data + pos);

                pool_elems.push_back(elem);
              }
            }

            switch (type) {
              case Layers::AVG_POOL: {
                val = 0;
                for (auto &e : pool_elems) {
                  val = op.plus(e, val);
                }
                T inverse = 1.f / pool_elems.size();
                val = op.mult(inverse, val);
              } break;
              case Layers::MAX_POOL: {
                val = -1;
                for (auto &e : pool_elems) {
                  val = val < e ? e : val;
                }
              } break;
              default:
                std::cerr
                    << "Cannot complete the operation. Pooling not supported"
                    << std::endl;
                break;
            }

            pos = x + y * target_width;
            pos += target_height * target_width * map;
            pos += target_height * target_width * target_channels * sample;
            *(output_data + pos) = val;
            pool_elems.clear();
          }
        }
      }
    }

    return Runtime{Runtime::OK, "Conv2D success"};
  }
};
}  // namespace Exact

/**
 * @brief Pooling 2D wrapper. It performs the validation and invokes the
 * execution kernel for pooling.
 *
 * @tparam T Datatype
 * @tparam K Execution kernel
 * @param ilayer Layer properties
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::Pooling<T>>
Runtime Pooling(const BasicLayer *ilayer, const DataContainer *input,
                DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::Pooling>;
  using QLayerType = QLayer<T, typename ::Pooling>;

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
  auto type = ilayer->type;

  if (!layer || (kQuantised && !qlayer)) {
    ret.msg = "Layer is not compatible with Pooling";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  int num_dims = layer->props.num_dimensions;
  if (num_dims != 2) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Cannot deal with poolings which are not 2D: pooling"};
  }

  /* Pooling info */
  int stride_x = layer->props.strides[0];
  int stride_y = layer->props.strides[1];
  int pooling_x = layer->props.poolings[0];
  int pooling_y = layer->props.poolings[1];

  /* Check image dimensions */
  num_dims = input->num_dimensions;
  if (num_dims != 4) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Cannot deal with poolings which are not 2D: image"};
  }

  int input_width = input->dimensions[0];
  int input_height = input->dimensions[1];
  int input_channels = input->dimensions[2];
  int input_samples = input->dimensions[3];
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  /* Compute output size */
  int target_width = ((input_width - pooling_x) / stride_x) + 1;
  int target_height = ((input_height - pooling_y) / stride_y) + 1;
  int target_channels = input_channels;
  int target_samples = input_samples;
  int64_t output_size =
      target_width * target_height * target_channels * target_samples;

  // TODO(lleon): add support for non-CPU implementations
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
  output->num_dimensions = 4;
  output->dimensions[0] = target_width;
  output->dimensions[1] = target_height;
  output->dimensions[2] = target_channels;
  output->dimensions[3] = target_samples;
  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  if (input->size < input_channels * input_height * input_width *
                        input_samples * sizeof(float)) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Input size is not completely accessible"};
  }

  if (output->size < target_channels * target_width * target_height *
                         target_samples * sizeof(float)) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Output size is not completely accessible"};
  }

  /* Prepare the kernel */
  execution_kernel.target_samples = target_samples;
  execution_kernel.target_channels = target_channels;
  execution_kernel.target_width = target_width;
  execution_kernel.target_height = target_height;
  execution_kernel.input_channels = input_channels;
  execution_kernel.input_width = input_width;
  execution_kernel.input_height = input_height;
  execution_kernel.type = type;
  execution_kernel.pooling_x = pooling_x;
  execution_kernel.stride_x = stride_x;
  execution_kernel.pooling_y = pooling_y;
  execution_kernel.stride_y = stride_y;

  /* Launch the kernel */
  return execution_kernel(input_data, output_data);
}
}  // namespace Kernels
