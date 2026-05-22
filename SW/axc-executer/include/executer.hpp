/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <accelerator.hpp>
#include <data.hpp>
#include <layer.hpp>
#include <runtime.hpp>

#include <memory>
#include <vector>

/**
 * @brief Executer
 *
 * It performs default SW fallback. The memory must be available to be in the SW
 * domain in some sort of unified fashion
 */
class Executer {
 public:
  /**
   * @brief Construct a new Executer object
   *
   * By default, it generates CPU-executable units. It allocates CPU or host
   * buffers. This class can be extended for accelerating layers. At the
   * moment, it only supports floating-point numbers.
   */
  Executer() = default;

  /**
   * @brief Construct a new Executer object
   *
   * By default, it generates CPU-executable units. It allocates CPU or host
   * buffers. This class can be extended for accelerating layers. At the
   * moment, it only supports floating-point numbers.
   * This constructor allows to pass custom accelerators to be taken into
   * account when running some layers.
   *
   * @param accels accelerators to add
   */
  explicit Executer(const std::vector<std::shared_ptr<IAccelerator>> &accels)
      : accelerators_{accels} {}

  /**
   * @brief Convolution 2D layer executer. If not extended, the Convolution 2D
   * executes on CPU
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Conv2D>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Conv2D(const std::shared_ptr<BasicLayer> ilayer,
                         const std::shared_ptr<DataContainer> input,
                         std::shared_ptr<DataContainer> output);

  /**
   * @brief Depthwise Convolution 2D layer executer. If not extended, the
   * Convolution 2D executes on CPU
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<DepthConv2D>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime DepthConv2D(const std::shared_ptr<BasicLayer> ilayer,
                              const std::shared_ptr<DataContainer> input,
                              std::shared_ptr<DataContainer> output);
  /**
   * @brief Dense layer executer. It performs fully connected layers on CPU
   * if not extended
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Dense>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Dense(const std::shared_ptr<BasicLayer> ilayer,
                        const std::shared_ptr<DataContainer> input,
                        std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs additions in 2D layers. Differently from Executer::Add,
   * it only supports 2D additions to add biases to the convolution layers
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Add>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Add2D(const std::shared_ptr<BasicLayer> ilayer,
                        const std::shared_ptr<DataContainer> input,
                        std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs additions in 1D layers. Differently from Executer::Add2D,
   * it only supports 1D additions to add biases to the dense layers
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Add>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Add(const std::shared_ptr<BasicLayer> ilayer,
                      const std::shared_ptr<DataContainer> input,
                      std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs multiplication in 1D layers.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Multiplier>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Multiplier(const std::shared_ptr<BasicLayer> ilayer,
                             const std::shared_ptr<DataContainer> input,
                             std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs multiplication in 2D layers.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Multiplier>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Multiplier2D(const std::shared_ptr<BasicLayer> ilayer,
                               const std::shared_ptr<DataContainer> input,
                               std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs pooling to the Convolutional layers. It can perform
   * average and max pooling on 2D layers.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Pooling>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Pooling(const std::shared_ptr<BasicLayer> ilayer,
                          const std::shared_ptr<DataContainer> input,
                          std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs padding to 2D layers.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Padding>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Padding(const std::shared_ptr<BasicLayer> ilayer,
                          const std::shared_ptr<DataContainer> input,
                          std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs layerwise operations.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Layerwise>
   * @param inputs input buffers
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Layerwise(
      const std::shared_ptr<BasicLayer> ilayer,
      const std::vector<std::shared_ptr<DataContainer>> inputs,
      std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs Activation Mapping
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<Mapper>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Activation(const std::shared_ptr<BasicLayer> ilayer,
                             const std::shared_ptr<DataContainer> input,
                             std::shared_ptr<DataContainer> output);

  /**
   * @brief Performs activations that require reduction of the terms such as
   * Softmax. Currently, Softmax is the only one supported by this executer.
   *
   * @param ilayer layer configuration. It specialises to
   * Layer<ReductionActivation>
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime ReductionActivation(
      const std::shared_ptr<BasicLayer> ilayer,
      const std::shared_ptr<DataContainer> input,
      std::shared_ptr<DataContainer> output);

  /**
   * @brief Runs a single layer. It analyses the layer interface and invokes
   * the proper executer based on the layer metadata.
   *
   * @param ilayer layer configuration with metadata.
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime RunLayer(const std::shared_ptr<BasicLayer> ilayer,
                           std::shared_ptr<DataContainer> input,
                           std::shared_ptr<DataContainer> output);

  /**
   * @brief Runs a model by decomposing the layers
   *
   * @param layers layer configuration with metadata coming from the model
   * @param input input buffer to execute on the model
   * @param output output buffer
   * @return Runtime
   */
  virtual Runtime Run(const std::vector<std::shared_ptr<BasicLayer>> &layers,
                      std::shared_ptr<DataContainer> input,
                      std::shared_ptr<DataContainer> output);

  /**
   * @brief Destroy the Executer object
   */
  virtual ~Executer() {}

 protected:
  /**
   * @brief Accelerators array with implementations
   *
   * Intended for subclassing
   */
  std::vector<std::shared_ptr<IAccelerator>> accelerators_;

  /**
   * @brief Supported layers in the executer
   *
   * Intended for subclassing
   */
  std::vector<Layers> supported_layers_;
};
