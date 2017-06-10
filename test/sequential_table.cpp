// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/sequential_table.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

constexpr std::size_t Size = 10;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) {
    std::cout << N << std::endl;
    ++results[N];
    PETRA_ASSERT(results[N] == 1);
  }
  void operator()(petra::InvalidInputError&&) noexcept { PETRA_ASSERT(false); }

  std::array<std::size_t, Size> results = {{0}};
};

template<typename S>
void run_test(S&& table) {
  for (std::size_t i = 0; i < Size; ++i) { table(i); }
}

int main() {
  { run_test(petra::make_sequential_table<Size>(test{})); }

  {
    constexpr auto test_with_error = [](auto&& i) noexcept {
      using T = decltype(i);
      if constexpr (petra::utilities::is_error_type<T>()) {
        return Size;
      } else {
        return std::decay_t<T>::value;
      }
    };
    auto table = petra::make_sequential_table<Size>(test_with_error);

    static_assert(noexcept(table(std::declval<std::size_t>())));

    run_test(table);
    // Try with an integer not in the set
    PETRA_ASSERT(table(20) == Size);
  }

  {
    // Try with a throwing callback
    auto test_with_exception = [](auto&& i) {
      using T = std::decay_t<decltype(i)>;
      if constexpr (petra::utilities::is_error_type<T>()) {
        throw std::runtime_error("Invalid input detected");
        return Size;
      } else {
        return T::value;
      }
    };
    auto table = petra::make_sequential_table<Size>(test_with_exception);
    static_assert(!noexcept(table(std::declval<std::size_t>())));
  }

  return 0;
}
