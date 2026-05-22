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
namespace lsbor {

/*
 * Multiply class template for template-specialization
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam or_bits bits to use for OR approximation
 */
template <class T, std::size_t size, std::size_t int_part, int or_bits,
          typename = void>
class Multiply;

/*
 * Signed Integer Multiply class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam or_bits bits to use for OR approximation
 */
template <class T, std::size_t size, std::size_t int_part, int or_bits>
class Multiply<T, size, int_part, or_bits,
               typename std::enable_if<std::is_integral<T>::value &&
                                       std::is_signed<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    constexpr auto msb_index = size - 1;
    constexpr auto lsb_index = or_bits - 1;
    constexpr auto reduced_size = size - or_bits;
    constexpr auto msb_size = 2 * reduced_size;

    static_assert(or_bits < size, "too many OR bits");
    static_assert(msb_index > 0, "size too small");
    static_assert(or_bits > 0, "too few OR bits");

    ap_int<reduced_size> lhs_msb = ap_int<size>(l).range(msb_index, or_bits);
    ap_int<reduced_size> rhs_msb = ap_int<size>(r).range(msb_index, or_bits);
    ap_int<msb_size> result_msb = lhs_msb * rhs_msb;

    ap_int<or_bits> lhs_lsb = ap_int<size>(l).range(or_bits, 0);
    ap_int<or_bits> rhs_lsb = ap_int<size>(r).range(or_bits, 0);
    ap_int<or_bits> result_lsb = lhs_lsb | rhs_lsb;

    ap_int<size> retval = 0;
    retval.range(msb_index, or_bits) =
        result_msb.range(msb_size - 1, reduced_size - 1);
    retval.range(or_bits, 0) = result_lsb;

    return retval;
  }
};

/*
 * Unsigned Integer Multiply class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam or_bits bits to use for OR approximation
 */
template <class T, std::size_t size, std::size_t int_part, int or_bits>
class Multiply<T, size, int_part, or_bits,
               typename std::enable_if<std::is_integral<T>::value &&
                                       std::is_unsigned<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    constexpr auto msb_index = size - 1;
    constexpr auto lsb_index = or_bits - 1;
    constexpr auto reduced_size = size - or_bits;
    constexpr auto msb_size = 2 * reduced_size;

    static_assert(or_bits < size, "too many OR bits");
    static_assert(msb_index > 0, "size too small");
    static_assert(or_bits > 0, "too few OR bits");

    ap_uint<reduced_size> lhs_msb = ap_uint<size>(l).range(msb_index, or_bits);
    ap_uint<reduced_size> rhs_msb = ap_uint<size>(r).range(msb_index, or_bits);
    ap_uint<msb_size> result_msb = lhs_msb * rhs_msb;

    ap_uint<or_bits> lhs_lsb = ap_uint<size>(l).range(or_bits, 0);
    ap_uint<or_bits> rhs_lsb = ap_uint<size>(r).range(or_bits, 0);
    ap_uint<or_bits> result_lsb = lhs_lsb | rhs_lsb;

    ap_uint<size> retval = 0;
    retval.range(msb_index, or_bits) =
        result_msb.range(msb_size - 1, reduced_size - 1);
    retval.range(or_bits, 0) = result_lsb;

    return retval;
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
 * Floating point Multiply class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam or_bits bits to use for OR approximation
 */
template <class T, std::size_t size, std::size_t int_part, int or_bits>
class Multiply<
    T, size, int_part, or_bits,
    typename std::enable_if<std::is_floating_point<T>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    constexpr auto lsb_index = or_bits - 1;
    constexpr auto msb_index = size - 1;
    constexpr auto reduced_size = size - or_bits;

    static_assert(or_bits < size, "too many OR bits");
    static_assert(msb_index > 0, "size too small");
    static_assert(or_bits > 0, "too few OR bits");

    auto lhs_msb =
        ap_fixed<reduced_size, int_part>(l).range(msb_index, or_bits);
    auto rhs_msb =
        ap_fixed<reduced_size, int_part>(r).range(msb_index, or_bits);
    auto resultmsb = lhs_msb * rhs_msb;

    auto lhs_lsb = ap_fixed<or_bits, 0>(l).range(lsb_index, 0);
    auto rhs_lsb = ap_fixed<or_bits, 0>(r).range(lsb_index, 0);
    auto resultlsb = lhs_lsb | rhs_lsb;

    ap_fixed<size, int_part> retval = 0;
    retval.range(msb_index, or_bits) = resultmsb;
    retval.range(lsb_index, 0) = resultlsb;

    return retval;
  }
};

/*
 * Arbitrary precision fixed point Multiply class
 * @tparam T datatype to work with.
 * @tparam size T's size in bits
 * @tparam int_part amount of bits to use as integer part for fixed point math
 * @tparam or_bits bits to use for OR approximation
 */
template <class T, std::size_t size, std::size_t int_part, int or_bits>
class Multiply<T, size, int_part, or_bits,
               typename std::enable_if<
                   std::is_same<T, ap_fixed<size, int_part>>::value>::type> {
 public:
  T operator()(T const l, T const r) const {
    constexpr auto reduced_size = size - or_bits;
    constexpr auto msb_index = size - 1;
    constexpr auto lsb_index = or_bits - 1;
    constexpr auto msb_size = 2 * reduced_size;
    constexpr auto frac_size = size - int_part;
    constexpr auto reduced_frac_size = frac_size - or_bits;

    static_assert(reduced_frac_size > 0, "fractional is smaller than dropped");
    static_assert(or_bits < size, "too many OR bits");
    static_assert(msb_index > 0, "size too small");
    static_assert(or_bits > 0, "too few OR bits");

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
    ap_int<reduced_size> lhs = l.range(msb_index, or_bits);
    ap_int<reduced_size> rhs = r.range(msb_index, or_bits);
    ap_int<msb_size> resultmsb = lhs * rhs;

    ap_int<or_bits> lhsor = l.range(lsb_index, 0);
    ap_int<or_bits> rhsor = r.range(lsb_index, 0);
    ap_int<or_bits> resultlsb = lhsor | rhsor;

    T retval = 0;
    retval.range(or_bits - 1, 0) = resultlsb;
    retval.range(msb_index, or_bits) =
        resultmsb.range(msb_size - 1, reduced_frac_size);
    return retval;
  }
};

}  // namespace lsbor
}  // namespace approximate
}  // namespace arithmetic
}  // namespace axc
