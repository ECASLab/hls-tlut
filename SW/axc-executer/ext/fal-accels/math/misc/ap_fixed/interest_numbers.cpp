#include <ap_fixed.h>

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <type_traits>

/* constexpr int int_part{ 1 }; */
constexpr int terminate_index{15};
constexpr int data_size{16};

void stuff_helper(std::integral_constant<int, terminate_index>) {}

template <int int_part>
void print(uint16_t const& s, ap_fixed<data_size, int_part> const& test) {
  std::cout << "s: 0x" << std::setfill('0') << std::setw(4) << std::uppercase
            << std::hex << s
            << "\t"
            /* << s << "\t" */
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
    /* uint16_t s; */
    std::array<uint16_t, 8> arr{
        {0x7FFFu, 0x8000u, 0x8001u, 0x8FFFu, 0xFFFFu, 0x0000u, 0x7000u}};
    /* for (uint16_t s = 0x8001u; s != 0x8000u; ++s) */
    for (auto const& s : arr) {
      constexpr auto msb_index = data_size - 1;

      a.range(msb_index, 0) = s;
      print<int_part>(s, a);
    }
  }
  stuff_helper(std::integral_constant<int, int_part + 1>());
}

int main(void) {
  stuff_helper();

  return 0;
}
