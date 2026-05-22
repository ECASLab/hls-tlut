/*
 * Copyright 2023
 * Author: Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>  // ap_fixed<> type
#include <ap_int.h>    // ap_[u]int<> types

#include <type_traits>  // std:: compile-time type checks

namespace axc {
namespace arithmetic {
namespace approximate {
namespace lsbfixed {

/*
 * Multiply class template for template-specialization
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits = 0,
          typename = void>
class Multiply;

/* Arbitrary precision fixed point Multiply class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits>
class Multiply<T, size, int_part, dropped_bits,
               typename std::enable_if<
                   std::is_same<T, ap_fixed<size, int_part>>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    constexpr auto reduced_size = size - dropped_bits;
    constexpr auto msb_index = size - 1;
    constexpr auto msb_size = 2 * reduced_size;
    constexpr auto frac_size = size - int_part;
    constexpr auto reduced_frac_size = frac_size - dropped_bits;
    constexpr int shifting = dropped_bits - 1 > 0 ? (dropped_bits - 1) : 0;
    constexpr int fixation = 1 << shifting;

    static_assert(dropped_bits < size, "too many dropped bits");
    static_assert(msb_index > 0, "size too small");
    static_assert(reduced_frac_size > 0, "fractional is smaller than dropped");
    static_assert(dropped_bits > 0, "too few dropped bits");
    /*
     * IMPORTANT: the use of ap_int<> here is deliberate, after testing either
     * typename T or ap_fixed<> directly, there appeared to be an issue with
     * the iplementation of its operator*() which caused overflowing data and
     * seemingly arbitrary results, but when using ap_int<>, and its range()
     * operator to set the specific bits of interest, the values stored in
     * memory seemed to be correct in the debugger. This might be seen as a
     * dependency on an implementation-level detail for the ap_int<> templated
     * class, but yields the expected behavior in Vivado 2018.2 in Ubuntu 18.04
     */
    ap_int<reduced_size> lhs = l.range(msb_index, dropped_bits);
    ap_int<reduced_size> rhs = r.range(msb_index, dropped_bits);
    /* multiplication requires double the size for the result type */
    ap_int<msb_size> result = lhs * rhs;

    T retval = fixation;
    /* multiplication requires taking the MSB of the double-sized result type */
    retval.range(msb_index, dropped_bits) = result.range(msb_size - 1, 
        reduced_frac_size);
    return retval;
  }
};

}  // namespace lsbdrop
}  // namespace approximate
}  // namespace arithmetic
}  // namespace axc
