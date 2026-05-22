/*
 * Copyright 2022
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 */

/* define LSBDROP as default, if no value is passed */
#define EXACT 0
#define DROP 1
#define OR 2

#if !defined(METHOD)
#define METHOD EXACT
#endif

#if !defined(AXC_BITS)
#define AXC_BITS 0
#endif

constexpr auto approximation_bits = AXC_BITS;

#if METHOD == DROP
#include "approximate/addition-lsbdrop.hpp"
#elif METHOD == OR
#include "approximate/addition-lsbor.hpp"
#else
#include "exact/addition.hpp"
#endif

#include "axc_add.hpp"

void axc_add(DataType const a, DataType const b, DataType& c) {
#if METHOD == DROP
  using namespace axc::arithmetic::approximate::lsbdrop;
  constexpr auto dropped_bits = approximation_bits;
  static auto adder = Add<DataType, Q_BW, Q_INT, dropped_bits>{};
#elif METHOD == OR
  using namespace axc::arithmetic::approximate::lsbor;
  constexpr auto or_bits = approximation_bits;
  static auto adder = Add<DataType, Q_BW, Q_INT, or_bits>{};
#else
  using namespace axc::arithmetic::exact;
  static auto adder = Add<DataType>{};
#endif

  c = adder(a, b);
}
