#include <ap_fixed.h>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <type_traits>

constexpr int terminate_index{15};
constexpr int data_size{16};

void stuff_helper(std::integral_constant<int, terminate_index>) {}

template <int int_part>
void print(uint16_t const& s, ap_fixed<data_size, int_part> const& test) {
  std::cout << "s: 0x" << std::setfill('0') << std::setw(4) << std::uppercase
            << std::hex << s << "\t"
            << "test: " << std::setfill(' ') << std::setw(12) << std::dec
            << test << std::endl;
}

template <int int_part = 0>
void stuff_helper(
    std::integral_constant<int, int_part> = std::integral_constant<int, 0>()) {
  {
    std::cout << "data_size: " << data_size << "; int_part: " << int_part
              << std::endl;
    ap_fixed<data_size, int_part> a = 0;
    ap_fixed<data_size, int_part> b = 0;
    /* for (uint16_t s = 0x8001u; s != 0x8000u; ++s) */
    {
      /* uint16_t s = 0x7FFFu; */
      /* uint16_t s = 0x67B0u; // 0.405029 */
      /* uint16_t s2 = 0x2DF0u; // 0.179443 */
      uint16_t s = 0x8FFFu;   // 0.405029
      uint16_t s2 = 0x8000u;  // 0.179443

      constexpr auto msb_index = data_size - 1;

      b.range(msb_index, 0) = s2;
      a.range(msb_index, 0) = s;

      print<int_part>(s, a);
      print<int_part>(s2, b);

      ap_fixed<data_size + 1, int_part> c = 0;
      c.range(msb_index, 0) = a + b;
      uint16_t s3 = c.range(msb_index, 0);

      print<int_part>(s3, c);

      ap_fixed<data_size, int_part> d = 0;
      uint16_t s4 = 0x8FFFu;  // -0.415527
      s4 ^= 0x8000;           // now s4 = 0.0844727 (0x15A0)
      d.range(msb_index, 0) = s4;

      print<int_part>(s4, d);

      /* s++; */
      /* a.range(data_size, 0) = s; */
      /* print (s, a); */
      /* a = 0.405029f; */
      /* a = 0.179443f; */
      /* s = a.range(data_size, 0); */
      /* a.range(data_size, 0) = s; */
      /* print (s, a); */
    }
  }
  /* do_stuff<int_part>(); */
  stuff_helper(std::integral_constant<int, int_part + 1>());
}

int main(void) {
  stuff_helper();

  return 0;
}
