/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <data.hpp>
#include <executer.hpp>
#include <kernel.hpp>
#include <layer.hpp>
#include <runtime.hpp>

#include <memory>
#include <string>
#include <vector>

/**
 * @brief FxPExecuter
 *
 * The fixed-point executer allows performing operations in fixed point using
 * the ap_fixed library from Vitis HLS
 */
template <class Q>
class FxPExecuter : public Executer {
 public:
  /**
   * @brief Construct a new Executer object
   *
   * By default, it generates CPU-executable units. It allocates CPU or host
   * buffers. This class can be extended for accelerating layers. At the
   * moment, it only supports floating-point numbers.
   */
  FxPExecuter() : Executer{} {
    supported_layers_ = {Layers::CONV2D, Layers::DENSE};
  };

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
  Runtime Conv2D(const std::shared_ptr<BasicLayer> ilayer,
                 const std::shared_ptr<DataContainer> input,
                 std::shared_ptr<DataContainer> output) override {
    return Kernels::Conv2D<Q>(ilayer.get(), input.get(), output.get());
  }

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
  Runtime Dense(const std::shared_ptr<BasicLayer> ilayer,
                const std::shared_ptr<DataContainer> input,
                std::shared_ptr<DataContainer> output) override {
    return Kernels::Dense<Q>(ilayer.get(), input.get(), output.get());
  }

  /**
   * @brief Destroy the Executer object
   */
  virtual ~FxPExecuter() = default;
};
