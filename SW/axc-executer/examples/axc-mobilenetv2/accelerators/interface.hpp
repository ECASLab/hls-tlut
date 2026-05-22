/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <memory>

#include <accelerator.hpp>
#include <layer.hpp>
#include <runtime.hpp>

/**
 * @brief Macro to add the constructor, destructor and statics to the
 * accelerator declaration
 *
 * Use this macro to avoid code replication when inheriting from the
 * IAccelerator interface
 */
#define BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM)                                   \
  static constexpr DataBits datawidth = BW;                                   \
  static constexpr IntBits intwidth = IW;                                     \
  static constexpr DropBits dropadd = DBA;                                    \
  static constexpr DataBits dropmult = DBM;                                   \
  explicit Accelerator(const AccelConfig &configs) : IAccelerator{configs} {} \
  virtual ~Accelerator() = default;

/**
 * @brief General accelerator specialisation
 *
 * This struct specialises the interface of IAccelerator. In this case, it
 * defaults the implementation, giving an error in case of not implementation.
 *
 * This struct is not intended for final use and it should be specialise in the
 * layer to execute. The specialisation is at template level, dropping the
 * parameters that are specialised. Thus, the existence of this general template
 * is an error source in case of misuse.
 *
 * @tparam L Layer to execute
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <Layers L, DataBits BW, IntBits IW, DropBits DBA, DropBits DBM,
          ArithApprox A>
struct Accelerator : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Execution routine
   *
   * Triggers the accelerator execution. It is responsibility of the
   * implementation to complement this with upload/download of the buffers
   * and parameters
   *
   * @return Runtime
   */
  Runtime Execute(const std::shared_ptr<BasicLayer>,
                  const std::shared_ptr<DataContainer>,
                  std::shared_ptr<DataContainer>) override {
    return Runtime{Runtime::NOT_IMPLEMENTED,
                   "The function has not been implemented"};
  }
};
