// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/sequential_table.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

// constexpr std::size_t Size = 10;
namespace petra {
}

template<auto Size>
struct test {
  using Integral = decltype(Size);
  template<Integral N>
  void operator()(std::integral_constant<Integral, N>&&) {
    std::cout << N << std::endl;
    ++results[petra::utilities::abs(N)];
    PETRA_ASSERT(results[petra::utilities::abs(N)] == 1);
  }
  void operator()(petra::InvalidInputError&&) noexcept { PETRA_ASSERT(false); }

  std::array<std::size_t, petra::utilities::abs(Size)> results = {{0}};
};

template<auto Size, typename S>
void run_test(S&& table) {
  if constexpr (std::is_signed<decltype(Size)>{} && Size < 0) {
    for (decltype(Size) i = 0; i > Size; --i) { table(i); }
  } else {
    for (decltype(Size) i = 0; i < Size; ++i) { table(i); }
  }
}

int main() {
  constexpr std::size_t USize = 10ul;
  {
    run_test<USize>(petra::make_sequential_table<USize>(test<USize>{}));
  }

  {
    constexpr auto test_with_error = [USize](auto&& i) noexcept {
      using T = decltype(i);
      if constexpr (petra::utilities::is_error_type<T>()) {
        return USize;
      } else {
        return std::decay_t<T>::value;
      }
    };
    auto table = petra::make_sequential_table<USize>(test_with_error);

    static_assert(noexcept(table(std::declval<std::size_t>())));

    run_test<USize>(table);
    // Try with an integer not in the set
    PETRA_ASSERT(table(20) == USize);
  }

  {
    // Try with a throwing callback
    auto test_with_exception = [USize](auto&& i) {
      using T = std::decay_t<decltype(i)>;
      if constexpr (petra::utilities::is_error_type<T>()) {
        throw std::runtime_error("Invalid input detected");
        return USize;
      } else {
        return T::value;
      }
    };
    auto table = petra::make_sequential_table<USize>(test_with_exception);
    static_assert(!noexcept(table(std::declval<std::size_t>())));
  }

  // Unsigned type
  {
    constexpr int SSize = 10;
    run_test<SSize>(petra::make_sequential_table<SSize>(test<SSize>{}));
  }

  // Negative sequence
  {
    constexpr int SSize = -10;
    run_test<SSize>(petra::make_sequential_table<SSize>(test<SSize>{}));
  }

  return 0;
}
