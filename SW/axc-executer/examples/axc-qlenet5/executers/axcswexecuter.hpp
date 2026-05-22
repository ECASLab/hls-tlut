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

#include "examples/common/kernels/arithmetic.hpp"

#include <memory>
#include <string>
#include <vector>

#ifndef AK_METHOD
#define AK_METHOD EXACT
#endif

/**
 * @brief AxCExecuter
 *
 * The fixed-point executer allows performing operations in fixed point using
 * the ap_fixed library from Vitis HLS
 *
 * @tparam Q quantised data type
 * @tparam L number of LSB bits to approximate
 */
template <class Q, int L = 2>
class AxCExecuter : public Executer {
#if AK_METHOD == LSBDROP
#warning "Using method LSBDROP"
  using ArithmeticEngine = Kernels::Arithmetic::ApproximateLsbDrop<Q, L>;
#elif AK_METHOD == LSBOR
#warning "Using method LSBOR"
  using ArithmeticEngine = Kernels::Arithmetic::ApproximateLsbOr<Q, L>;
#elif AK_METHOD == LSBFIXED
#warning "Using method LSBFIXED"
  using ArithmeticEngine = Kernels::Arithmetic::ApproximateLsbFixed<Q, L>;
#else
#warning "Invalid method. Falling back to EXACT"
  using ArithmeticEngine = Kernels::Arithmetic::Exact<Q>;
#endif

 public:
  /**
   * @brief Construct a new Executer object
   *
   * By default, it generates CPU-executable units. It allocates CPU or host
   * buffers. This class can be extended for accelerating layers. At the
   * moment, it only supports floating-point numbers.
   */
  AxCExecuter() : Executer{} {
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
    using KernelEngine = Kernels::Exact::Conv2D<Q, ArithmeticEngine>;
    return Kernels::Conv2D<Q, KernelEngine>(ilayer.get(), input.get(),
                                            output.get());
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
    using KernelEngine = Kernels::Exact::Dense<Q, ArithmeticEngine>;
    return Kernels::Dense<Q, KernelEngine>(ilayer.get(), input.get(),
                                           output.get());
  }

  /**
   * @brief Destroy the Executer object
   */
  virtual ~AxCExecuter() = default;
};
