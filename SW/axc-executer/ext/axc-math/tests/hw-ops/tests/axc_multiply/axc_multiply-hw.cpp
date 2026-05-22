/*
 * Copyright 2022-2023
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 *         Luis G. Leon Vega <luis.leon@ieee.org>
 */

/* define LSBDROP as default, if no value is passed */
#define EXACT 0
#define DROP 1
#define OR 2
#define FIXED 3

#if !defined(METHOD)
#define METHOD EXACT
#endif

#if !defined(AXC_BITS)
#define AXC_BITS 0
#endif

constexpr auto approximation_bits = AXC_BITS;

#if METHOD == DROP
#include "approximate/multiplication-lsbdrop.hpp"
#elif METHOD == OR
#include "approximate/multiplication-lsbor.hpp"
#elif METHOD == FIXED
#include "approximate/multiplication-lsbfixed.hpp"
#else
#include "exact/multiplication.hpp"
#endif

#include "axc_multiply.hpp"

void axc_multiply(const DataType a, const DataType b, DataType& c) {
#if METHOD == DROP
  using namespace axc::arithmetic::approximate::lsbdrop;
  constexpr auto dropped_bits = approximation_bits;
  static auto multiplier = Multiply<DataType, Q_BW, Q_INT, dropped_bits>{};
#elif METHOD == OR
  using namespace axc::arithmetic::approximate::lsbor;
  constexpr auto or_bits = approximation_bits;
  static auto multiplier = Multiply<DataType, Q_BW, Q_INT, or_bits>{};
#elif METHOD == FIXED
  using namespace axc::arithmetic::approximate::lsbfixed;
  constexpr auto fixed_bits = approximation_bits;
  static auto multiplier = Multiply<DataType, Q_BW, Q_INT, fixed_bits>{};
#else
  using namespace axc::arithmetic::exact;
  static auto multiplier = Multiply<DataType>{};
#endif

  c = multiplier(a, b);
}
