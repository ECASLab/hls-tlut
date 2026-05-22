/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <memory>
#include <tuple>

#include <approximate.hpp>
#include <data.hpp>
#include <layer.hpp>

/**
 * @brief Typification for the IntBits DoF
 */
using IntBits = int;

/**
 * @brief Typification for the DataBits DoF
 */
using DataBits = int;

/**
 * @brief Typification for the DropBits DoF
 */
using DropBits = int;

/**
 * @brief Alias for the Accelerator Configuration
 */
using AccelConfig =
    std::tuple<Layers, DataBits, IntBits, DropBits, DropBits, ArithApprox>;

/**
 * @brief Accelerator Interface
 *
 * This struct summarises the accelerator as a functor. It contains an
 * IAccelerator::Execute method to trigger the "accelerator execution".
 * The implementation is up to implementation.
 */
struct IAccelerator {
  /**
   * @brief Construct a new IAccelerator object
   *
   * Constructor deleted since each accelerator must have configurations
   * associated to it.
   */
  IAccelerator() = delete;

  /**
   * @brief Construct a new IAccelerator object
   *
   * Custom constructor to add configs to the accelerator. Each accelerator
   * must be built with this constructor.
   * @param configs configs to set
   */
  explicit IAccelerator(const AccelConfig &configs) : configs_{configs} {}

  /**
   * @brief Execution routine
   *
   * Triggers the accelerator execution. It is responsibility of the
   * implementation to complement this with upload/download of the buffers
   * and parameters
   *
   * @return Runtime
   */
  virtual Runtime Execute(const std::shared_ptr<BasicLayer>,
                          const std::shared_ptr<DataContainer>,
                          std::shared_ptr<DataContainer>) = 0;

  /**
   * @brief Destroy the IAccelerator object
   */
  virtual ~IAccelerator() = default;

  /**
   * @brief Get the accelerator configs
   *
   * @return const AccelConfig& configs of the accelerator
   */
  virtual const AccelConfig &GetConfigs() const noexcept { return configs_; }

 protected:
  /**
   * @brief Configurations of the accelerator
   */
  const AccelConfig configs_;
};
