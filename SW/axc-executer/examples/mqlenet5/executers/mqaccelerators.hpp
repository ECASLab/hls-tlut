/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>

#include <accelerator.hpp>
#include <approximate.hpp>
#include <iostream>
#include <kernel.hpp>
#include <layer.hpp>
#include <memory>
#include <runtime.hpp>
#include <string>
#include <tuple>
#include <type_traits>

#include "examples/common/kernels/arithmetic.hpp"
#include "tluttanh.hpp"  // NOLINT
#include "tlut.hpp"  // NOLINT

/**
 * @brief Macro to select the arithmetic typing at compilation time
 *
 * This allows the accelerator simulator arithmetic choose. In practice,
 * this should not be necessary since it is inherent to what's implemented
 * on the accelerator
 */
#define GET_ARITH_ENGINE(A, T, LA, LM)                             \
  typename std::conditional<                                       \
      (A) == ArithApprox::LSBDROP_A,                               \
      Kernels::Arithmetic::ApproximateLsbDrop<T, LA, LM>,          \
      typename std::conditional<                                   \
          (A) == ArithApprox::LSBOR_A,                             \
          Kernels::Arithmetic::ApproximateLsbOr<T, LA, LM>,        \
          typename std::conditional<                               \
              (A) == ArithApprox::LSBFIXED_A,                      \
              Kernels::Arithmetic::ApproximateLsbFixed<T, LA, LM>, \
              Kernels::Arithmetic::Exact<T> >::type>::type>::type

/**
 * @brief Macro to select the arithmetic operators at compilation time
 *
 * This allows the accelerator simulator arithmetic choose. In practice,
 * this should not be necessary since it is inherent to what's implemented
 * on the accelerator
 */
#define GET_ARITH_OPERATOR(A, T, L, OP)                                       \
  typename std::conditional<                                                  \
      (A) == ArithApprox::LSBDROP_A,                                          \
      axc::arithmetic::approximate::lsbdrop::OP<T, T::width, T::iwidth, L>,   \
      typename std::conditional<                                              \
          (A) == ArithApprox::LSBOR_A,                                        \
          axc::arithmetic::approximate::lsbor::OP<T, T::width, T::iwidth, L>, \
          typename std::conditional<                                          \
              (A) == ArithApprox::LSBFIXED_A,                                 \
              axc::arithmetic::approximate::lsbfixed::OP<T, T::width,         \
                                                         T::iwidth, L>,       \
              axc::arithmetic::exact::OP<T> >::type>::type>::type

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

/* -------------- Specialisations ---------------- */

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
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
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
    using namespace Kernels::Arithmetic;  // NOLINT
    using ArithmeticEngine = GET_ARITH_ENGINE(A, Q, DBA, DBM);
    using KernelEngine = Kernels::Exact::Conv2D<Q, ArithmeticEngine>;
    std::cout << "Running on Accelerator of CONV2D with arithmetic "
              << ArithApproxStr[static_cast<int>(A)] << " ( " << BW << " , "
              << IW << " , " << DBA << " , " << DBM << " )" << std::endl;
    return Kernels::Conv2D<Q, KernelEngine>(ilayer.get(), input.get(),
                                            output.get());
  }
};

/**
 * @brief Dense Accelerator Simulator
 *
 * This simulates a dense accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration.
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact dense kernel.
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::DENSE, BW, IW, DBA, DBM, A> : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the dense engine
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
    using ArithmeticEngine = GET_ARITH_ENGINE(A, Q, DBA, DBM);
    using KernelEngine = Kernels::Exact::Dense<Q, ArithmeticEngine>;
    std::cout << "Running on Accelerator of DENSE with arithmetic "
              << ArithApproxStr[static_cast<int>(A)] << " ( " << BW << " , "
              << IW << " , " << DBA << " , " << DBM << " )" << std::endl;
    return Kernels::Dense<Q, KernelEngine>(ilayer.get(), input.get(),
                                           output.get());
  }
};

/**
 * @brief Pooling Accelerator Simulator
 *
 * This simulates a pooling accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration.
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact pooling kernel.
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::AVG_POOL, BW, IW, DBA, DBM, A>
    : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the pooling engine
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
    using ArithmeticEngine = GET_ARITH_ENGINE(A, Q, DBA, DBM);
    using KernelEngine = Kernels::Exact::Pooling<Q, ArithmeticEngine>;
    std::cout << "Running on Accelerator of POOLING with arithmetic "
              << ArithApproxStr[static_cast<int>(A)] << " ( " << BW << " , "
              << IW << " , " << DBA << " , " << DBM << " )" << std::endl;
    return Kernels::Pooling<Q, KernelEngine>(ilayer.get(), input.get(),
                                             output.get());
  }
};

/**
 * @brief Addition Accelerator Simulator
 *
 * This simulates a addition accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration.
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact addition kernel.
 *
 * It supports 1D and 2D Addition
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::ADD, BW, IW, DBA, DBM, A> : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the addition engine
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
    using LayerType = Layer<typename ::Add>;
    auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
    Layers preceding = layer->props.preceding;

    using Q = ap_fixed<BW, IW>;
    using ArithmeticEngine = GET_ARITH_OPERATOR(A, Q, DBA, Add);
    using NonLinearEngine = Kernels::NonLinear::STL<Q>;

    switch (preceding) {
      case Layers::CONV2D: {
        using KernelEngine =
            Kernels::Exact::ElementWise2D_TLUT<Q, ArithmeticEngine>;
        std::cout << "Running on Accelerator of Add2D with arithmetic "
                  << ArithApproxStr[static_cast<int>(A)] << " ( " << BW << " , "
                  << IW << " , " << DBA << " , " << DBM << " )" << std::endl;
        return Kernels::ElementWise2D<Q, KernelEngine, ::Add>(
            ilayer.get(), input.get(), output.get());
      } break;
      case Layers::DENSE: {
        using KernelEngine =
            Kernels::Exact::ElementWise_TLUT<Q, ArithmeticEngine>;
        std::cout << "Running on Accelerator of Add1D with arithmetic "
                  << ArithApproxStr[static_cast<int>(A)] << " ( " << BW << " , "
                  << IW << " , " << DBA << " , " << DBM << " )" << std::endl;
        return Kernels::ElementWise<Q, KernelEngine, ::Add>(
            ilayer.get(), input.get(), output.get());
      } break;
      default:
        return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                       "Unsupported layer preceding an addition"};
        break;
    }
  }
};

/**
 * @brief Multiplication Accelerator Simulator
 *
 * This simulates a multiplication accelerator. This simulator accepts any kind
 * of approximation at any degree. It is intended for performing design space
 * exploration.
 *
 * It implements the Arithmetic Engine with approximation provided by A,
 * at the degrees DBA and DBM and the data type specified in BW and IW.
 * Everything is condensated in an exact multiplication kernel.
 *
 * It supports 1D and 2D Multiplication
 *
 * @tparam BW Data width in bits
 * @tparam IW Integer width in bits
 * @tparam DBA Bits to drop in the adder
 * @tparam DBM Bits to drop in the multiplier
 * @tparam A Approximation type given ArithApprox
 */
template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::MULTIPLIER, BW, IW, DBA, DBM, A>
    : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the multiplication engine
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
    using LayerType = Layer<typename ::Add>;
    auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
    Layers preceding = layer->props.preceding;

    using Q = ap_fixed<BW, IW>;
    using ArithmeticEngine = GET_ARITH_OPERATOR(A, Q, DBM, Multiply);
    using NonLinearEngine = Kernels::NonLinear::STL<Q>;

    using KernelEngine =
        Kernels::Exact::ElementWise<Q, NonLinearEngine, ArithmeticEngine>;
    std::cout << "Running on Accelerator of Multiplier with arithmetic "
              << "exact" << " ( " << BW << " , " << IW << " , " << DBA << " , "
              << DBM << " )" << std::endl;
    return Kernels::ElementWise<Q, KernelEngine, ::Multiplier>(
        ilayer.get(), input.get(), output.get());
  }
};

template <DataBits BW, IntBits IW, DropBits DBA, DropBits DBM, ArithApprox A>
struct Accelerator<Layers::REDUCT_ACTIVATION, BW, IW, DBA, DBM, A>
    : public IAccelerator {
  /* Add the declarations */
  BEGIN_ACCEL_DECLS(BW, IW, DBA, DBM);

  /**
   * @brief Triggers the execution of the multiplication engine
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
    using LayerType = Layer<typename ::Add>;
    auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
    Layers preceding = layer->props.preceding;

    using Q = ap_fixed<BW, IW>;
    using ArithmeticEngine = GET_ARITH_OPERATOR(A, Q, DBM, Multiply);
    using NonLinearEngine = Kernels::NonLinear::STL<Q>;

    using KernelEngine = Kernels::Exact::SoftmaxTLUT<Q>;
    std::cout << "Running on Accelerator of Softmax with arithmetic " << "exact"
              << " ( " << BW << " , " << IW << " , " << DBA << " , " << DBM
              << " )" << std::endl;

    return Kernels::ReductionActivation<Q, KernelEngine>(
        ilayer.get(), input.get(), output.get());
  }
};
