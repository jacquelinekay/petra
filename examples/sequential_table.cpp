#include "petra/sequential_table.hpp"
#include "petra/utilities.hpp"

#include <array>
#include <cassert>
#include <iostream>

static auto fill_array(petra::InvalidInputError&&) {
  throw std::runtime_error("Value exceeded maximum array size.");
}

template<typename Integral, Integral N>
static auto fill_array(std::integral_constant<Integral, N>&&) {
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
  if (x > 10) {
    std::cout << "Sorry, array sizes over 10 are not supported.\n";
    return 255;
  }

  auto get_result = petra::make_sequential_table<std::size_t, 10>(
    [](auto&& i) {
      return fill_array(std::forward<std::decay_t<decltype(i)>>(i));
    });

  get_result(x);

  return 0;
}
