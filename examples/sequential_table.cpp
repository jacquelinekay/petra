#include "petra/sequential_table.hpp"

#include <array>
#include <cassert>
#include <iostream>

template<std::size_t N>
static constexpr auto fill_array(std::integral_constant<std::size_t, N>&&) {
  std::array<int, N> buckets;
  /* ... */
  (void)buckets;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Please enter an array size!\n";
    return 255;
  }

  std::size_t x = std::stoul(argv[1]);
  if (x >= 10) {
    std::cout << "Sorry, only array sizes under 10 are supported.\n";
    return 255;
  }

  constexpr auto get_result =
    petra::make_sequential_table<10>(
      [](auto&& i){
        return fill_array(std::forward<decltype(i)>(i));
      });
  get_result(x);

  return 0;
}
