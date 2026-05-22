/*
 * Copyright 2023
 * Author: Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <ratio>

#include <operators.hpp>

/* Define Cores */
#define EXACT 0
#define LUT 1
#define TAYLOR 2
#define EXP_BASED 3

#include "axc_nl.hpp"

#define CORE(x, a) op.x((a))

void axc_nl(const DataType a, DataType& b) {
  using kMin = std::ratio<Q_MIN, 1>;
  using kMax = std::ratio<Q_MAX, 1>;
#pragma HLS inline off

#if METHOD == EXP_BASED
  using ExpOp = axc::nonlinear::approximate::lut::Exponential<DataType, kMin,
                                                              kMax, Q_POINTS>;
  axc::nonlinear::approximate::exp::TanH<DataType, ExpOp> op;
#elif METHOD == LUT
  axc::nonlinear::approximate::LUTMath<DataType, kMin, kMax, Q_POINTS> op;
#elif METHOD == TAYLOR
  axc::nonlinear::approximate::TaylorMath<DataType, Q_ORDER> op;
#else
  axc::nonlinear::exact::Exact<DataType> op;
#endif

#if METHOD == EXP_BASED
  b = op(a);
#else
  b = CORE(Q_NL_OP, a);
#endif
}
