#include "petra/sequential_table.hpp"
#include "petra/utilities.hpp"

#include <array>
#include <cassert>
#include <iostream>

template<auto N>
static constexpr auto
fill_array(std::integral_constant<decltype(N), N>&&) noexcept {
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

  constexpr auto get_result = petra::make_sequential_table<10>([](auto&& i) {
    using T = decltype(i);
    if constexpr (petra::utilities::is_error_type<T>()) {
      throw std::runtime_error("Value exceeded maximum array size.");
    } else {
      // Increase the input type by one.
      return fill_array(
          std::integral_constant<std::size_t, std::decay_t<T>::value>{});
    }
  });

  get_result(x);

  return 0;
}
