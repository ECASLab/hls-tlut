/*
 * Copyright 2022
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>  // ap_fixed<> type
#include <ap_int.h>    // ap_[u]int<> types

#include <type_traits>  // std:: compile-time type checks

namespace axc {
namespace arithmetic {
namespace approximate {
namespace lsbdrop {

/*
 * Add class template for template-specialization
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits = 0,
          typename = void>
class Add;

/*
 * Signed Integer Add class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits>
class Add<T, size, int_part, dropped_bits,
          typename std::enable_if<std::is_integral<T>::value &&
                                  std::is_signed<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    static_assert(dropped_bits < size, "too many dropped bits");
    constexpr auto reduced_size = size - dropped_bits;
    constexpr auto msb_index = size - 1;
    static_assert(msb_index > 0, "size too small");
    ap_int<size> result;
    ap_int<reduced_size> lhs = ap_int<size>(l).range(msb_index, dropped_bits);
    ap_int<reduced_size> rhs = ap_int<size>(r).range(msb_index, dropped_bits);
    result.range(msb_index, dropped_bits) = lhs + rhs;
    return result;
  }
};

/*
 * Unsigned Integer Add class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits>
class Add<T, size, int_part, dropped_bits,
          typename std::enable_if<std::is_integral<T>::value &&
                                  std::is_unsigned<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    static_assert(dropped_bits < size, "too many dropped bits");
    constexpr auto reduced_size = size - dropped_bits;
    constexpr auto msb_index = size - 1;
    static_assert(msb_index > 0, "size too small");
    ap_uint<size> result;
    ap_uint<reduced_size> lhs = ap_uint<size>(l).range(msb_index, dropped_bits);
    ap_uint<reduced_size> rhs = ap_uint<size>(r).range(msb_index, dropped_bits);
    result.range(msb_index, dropped_bits) = lhs + rhs;
    return result;
  }
};

/*
 * from:
 * docs.xilinx.com/r/en-US/ug1399-vitis-hls/Overview-of-Arbitrary-Precision-Fixed-Point-Data-Types
 *
 * Fixed-point data types model the data as an integer and fraction bits with
 * the format ap_fixed<W,I,[Q,O,N]> as explained in the table below. In the
 * following example, the Vitis HLS ap_fixed type is used to define an 18-bit
 * variable with 6 bits specified as representing the numbers above the binary
 * point, and 12 bits implied to represent the value after the decimal point.
 * The variable is specified as signed and the quantization mode is set to
 * round to plus infinity. Because the overflow mode is not specified, the
 * default wrap-around mode is used for overflow.
 *
 * ap_fixed<18,6,AP_RND > my_type;
 * ap_fixed<W,I,Q,O,N>
 * W: Word length in bits
 * I: bits to represent
 * Q: Quantization mode
 * O: Overflow mode
 * N: number of saturation bits in overflow wrap modes;
 */

/*
 * Floating point Add class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits>
class Add<T, size, int_part, dropped_bits,
          typename std::enable_if<std::is_floating_point<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    static_assert(dropped_bits < size, "too many dropped bits");
    constexpr auto msb_index = size - 1;
    static_assert(msb_index > 0, "size too small");
    auto lhs = ap_fixed<size, int_part>(l).range(msb_index, dropped_bits);
    auto rhs = ap_fixed<size, int_part>(r).range(msb_index, dropped_bits);
    auto result = lhs + rhs;
    return result;
  }
};

/*
 * Arbitrary precision fixed point Add class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam dropped_bits bits to ignore for approximation
 */
template <class T, std::size_t size, std::size_t int_part, int dropped_bits>
class Add<T, size, int_part, dropped_bits,
          typename std::enable_if<
              std::is_same<T, ap_fixed<size, int_part>>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    static_assert(dropped_bits < size, "too many dropped bits");
    constexpr auto reduced_size = size - dropped_bits;
    constexpr auto msb_index = size - 1;
    static_assert(msb_index > 0, "size too small");
    /*
     * IMPORTANT: the use of ap_int<> here is deliberate, after testing either
     * typename T or ap_fixed<> directly, there appeared to be an issue with
     * the iplementation of its operator+() which caused overflowing data and
     * seemingly arbitrary results, but when using ap_int<>, and its range()
     * operator to set the specific bits of interest, the values stored in
     * memory seemed to be correct in the debugger. This might be seen as a
     * dependency on an implementation-level detail for the ap_int<> templated
     * class, but yields the expected behavior in Vivado 2018.2 in Ubuntu 18.04
     */
    ap_int<reduced_size> lhs = l.range(msb_index, dropped_bits);
    ap_int<reduced_size> rhs = r.range(msb_index, dropped_bits);
    ap_int<reduced_size> result = lhs + rhs;
    T retval = 0;
    retval.range(msb_index, dropped_bits) = result.range(reduced_size - 1, 0);
    return retval;
  }
};

}  // namespace lsbdrop
}  // namespace approximate
}  // namespace arithmetic
}  // namespace axc
