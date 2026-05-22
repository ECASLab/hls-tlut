/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <data.hpp>
#include <properties.hpp>
#include <runtime.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Maximum number of dimensions of the layer of a model and the buffer
 */
static constexpr int kMaxNumDims = 5;

/**
 * @brief Layer enumerator to determine the type of a layer
 */
enum class Layers {
  CONV2D,            /** Convolution 2D */
  DENSE,             /** Dense layer */
  ADD,               /** Add layer [used to add biases] */
  MAX_POOL,          /** Max pooling in 2D */
  AVG_POOL,          /** Average pooling in 2D */
  REDUCT_ACTIVATION, /** Reduce Activation for complex activations */
  MULTIPLIER,        /** Multiplier layer [user for scaling or element-wise] */
  DEPTHWISE_CONV2D,  /** Depthwise convolution 2D layer */
  ACTIVATION,        /** Element-wise mapper using an activation function */
  PADDING,           /** Padding 2D */
  LAYERWISE,         /** Layerwise operations */
  INVALID
};

/**
 * @brief Map with the names in string for the Layers enumerator
 */
static std::unordered_map<int, std::string> LayerNames = {
    {static_cast<int>(Layers::DEPTHWISE_CONV2D), "DepthConv2D"},
    {static_cast<int>(Layers::CONV2D), "Conv2D"},
    {static_cast<int>(Layers::DENSE), "Dense"},
    {static_cast<int>(Layers::ADD), "Add"},
    {static_cast<int>(Layers::MULTIPLIER), "Multiplier"},
    {static_cast<int>(Layers::MAX_POOL), "MaxPooling"},
    {static_cast<int>(Layers::AVG_POOL), "AveragePooling"},
    {static_cast<int>(Layers::ACTIVATION), "Activation"},
    {static_cast<int>(Layers::PADDING), "Padding"},
    {static_cast<int>(Layers::LAYERWISE), "Layerwise"},
    {static_cast<int>(Layers::REDUCT_ACTIVATION), "ReductionActivation"}};

/**
 * @brief Activation enumerator to determine the class of activation
 */
enum class Activations {
  NONE = 0, /** Without activation or pass-thru */
  TANH,     /** Hyperbolic Tangent activation */
  RELU,     /** Rectifier Linear Unit activation */
  SOFTMAX   /** Softmax activation [analogous to the logistic] */
};

/**
 * @brief Convolution 2D property struct
 *
 * It holds the properties and configuration for the convolution layer
 */
struct Conv2D {
  int num_dimensions;           /** Number of dimensions of the layer */
  int dimensions[kMaxNumDims];  /** Elements per dimension */
  int strides[kMaxNumDims];     /** Strides of the convolution */
  int padding[kMaxNumDims];     /** Padding added to the input */
  int dilatations[kMaxNumDims]; /** Dilatation of the convolution */
};

/**
 * @brief Depthwise Convolution 2D property struct
 *
 * It holds the properties and configuration for the convolution layer
 */
using DepthConv2D = Conv2D;

/**
 * @brief Dense property struct
 *
 * It holds the properties and configuration for the dense layer
 */
struct Dense {
  int num_dimensions;          /** Number of dimensions of the layer */
  int dimensions[kMaxNumDims]; /** Elements per dimension */
};

/**
 * @brief Add layer property struct
 *
 * It holds the properties and configuration for the add layer (Add and Add2D)
 */
struct Add {
  /** Number of dimensions of the layer */
  int num_dimensions;
  /** Elements per dimension */
  int dimensions[kMaxNumDims];
  /** Preceding layer to decide if Add1D or Add2D should be performed*/
  Layers preceding;
};

/**
 * @brief Multiplier layer property struct
 *
 * It holds the properties and configuration for the mult layer (Multiplier)
 */
using Multiplier = Add;

/**
 * @brief Pooling property struct
 *
 * It holds the properties for the pooling
 */
struct Pooling {
  int num_dimensions;        /** Number of dimensions of the layer */
  int poolings[kMaxNumDims]; /** Pooling size */
  int strides[kMaxNumDims];  /** Striding of the pooling kernel */
};

/**
 * @brief Reduction activation property
 *
 * It just specialises the layer and do not hold any special property
 */
struct ReductionActivation {};

/**
 * @brief Mapper property
 *
 * It specialises to the mapper layer
 */
struct Mapper {
  float minvalue = -1e6; /** Minimum value to threshold */
  float maxvalue = 1e6;  /** Maximum value to threshold */
};

/**
 * @brief Padding property
 *
 * It specialises to the padding layer
 */
struct Padding {
  int num_dimensions; /** Number of dimensions of the layer */
  /** Padding added to the input */
  int padding[kMaxNumDims];
  /** Preceding layer to decide if Add1D or Add2D should be performed*/
  Layers preceding;
};

/**
 * @brief Layerwise properties
 *
 * It specialises according to the layer
 */
struct Layerwise {
  /** Initial value to write on the outputs */
  float initial_value = 0;
  /** Layer operation */
  Layers operation;
};

/**
 * @brief Activation property
 */
struct Activation : public Mapper {};

/**
 * @brief BasicLayer class
 *
 * It defines the minimum properties that must be defined to configure a layer
 * such as the activation, layer type, sizes, and the data (if it has
 * parameters)
 */
struct BasicLayer {
  /** Activation at the end of the computation */
  Activations activation;
  /** Layer type from Layers */
  Layers type;
  /** Size of the parameters in bytes */
  uint64_t size;
  /** Input elements that can fit the layer */
  int64_t input_size;
  /** Output elements that outcome from the computation */
  int64_t output_size;
  /** Number of required inputs */
  uint32_t num_inputs;
  /** Number of required outputs */
  uint32_t num_outputs;
  /** Parameters data */
  std::shared_ptr<float> data{nullptr};
  /** Custom execution props */
  std::unordered_map<CustomProps, CustomPropsPayload> custom_props;
  /** Input layers: layers that connect to this layer inputs  */
  std::vector<std::shared_ptr<BasicLayer>> input_layers;
  /** Output layers: layers that use this layer outputs as inputs */
  std::vector<std::shared_ptr<BasicLayer>> output_layers;
  /** Output: data container that saves the outputs of the layer */
  std::shared_ptr<DataContainer> output;

  /**
   * @brief Quantises the Layer
   *
   * It quantises the weights and it is used when loading the parameters
   *
   * @return Runtime
   */
  virtual Runtime Quantise();

  /**
   * @brief Connects the layers to form a graph
   *
   * It aggregates the input layers to extract the buffers from and the ones
   * that consumes the outputs from the current layer
   *
   * @param inputs input layers to connect
   * @param outputs output layers to connect
   * @return Runtime
   */
  virtual Runtime Connect(std::vector<std::shared_ptr<BasicLayer>> inputs,
                          std::vector<std::shared_ptr<BasicLayer>> outputs);

  /**
   * @brief Get the Container object
   *
   * This quantises the incoming DataContainer to the correct type
   * compatible with the layer.
   *
   * The use of shared pointers is justified by the complexity of handling the
   * cases where:
   *
   * - The layer is not quantised: so, the reference must preserve without
   *   deallocation
   * - The layer is quantised and the buffer is compatible: the reference is
   *   also preserved
   * - The layer is quantised and the buffer is incompatible: dequantise and
   *   quantise again (or requantised)
   *
   * @param in incoming buffer to convert
   * @return DataContainer* [Full Transfer] output container. The user is in
   * charge of deallocating the buffer. When there is no quantisation, the
   * shared_ptr behaves like a reference
   */
  virtual std::shared_ptr<DataContainer> GetContainer(
      std::shared_ptr<DataContainer> in);

  /**
   * @brief Destroy the Basic Layer object. Required for extension
   */
  virtual ~BasicLayer() = default;
};

/**
 * @brief Layer specialisation template
 *
 * It specialises the layer to content layer-specific properties. It extends
 * the BasicLayer class to include additional properties required for a
 * specific layer
 *
 * @tparam Props Class to the layer-specific properties
 */
template <class Props>
struct Layer : public BasicLayer {
  Props props; /** Layer-specific properties */

  virtual ~Layer() = default;
};

inline Runtime BasicLayer::Quantise() {
  return Runtime{Runtime::OK, "No-quantisation implemented"};
}

inline std::shared_ptr<DataContainer> BasicLayer::GetContainer(
    std::shared_ptr<DataContainer> in) {
  in->Dequantise();
  return in;
}

inline Runtime BasicLayer::Connect(
    std::vector<std::shared_ptr<BasicLayer>> inputs,
    std::vector<std::shared_ptr<BasicLayer>> outputs) {
  input_layers.insert(input_layers.begin(), inputs.begin(), inputs.end());
  output_layers.insert(output_layers.begin(), outputs.begin(), outputs.end());
  return Runtime{Runtime::OK, "Added"};
}
