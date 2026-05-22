/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <iostream>
#include <memory>

#include <accelerator.hpp>
#include <kernel.hpp>
#include <layer.hpp>
#include <runtime.hpp>

#include "./kernels/conv2d-24-8.hpp"
#include "interface.hpp"  // NOLINT

/**
 * @brief 2D Convolution Accelerator Simulator
 *
 * This simulates a convolution accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration.
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact convolution kernel.
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder [unused]
 * @tparam DBM Bits to drop in the multiplier [unused]
 * @tparam A Approximation type given ArithApprox [unused]
 */
template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::CONV2D, BW, IW, DBA, DBM, A> : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the convolution
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
    using Q = ap_fixed<BW, IW>;
    using KernelEngine = Kernels::FAL::Conv2D_Spatial_24_8;
    std::cout << "Running on Accelerator of CONV2D with custom kernel"
              << " ( " << BW << " , " << IW << " )" << std::endl;
    return Kernels::Conv2D<Q, KernelEngine>(ilayer.get(), input.get(),
                                            output.get());
  }
};
