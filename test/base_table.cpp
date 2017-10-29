// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/base_table.hpp"
#include "utilities.hpp"
#include "petra/utilities.hpp"

#include <array>
#include <iostream>

template<auto Size>
struct test {
  using Integral = decltype(Size);
  template<Integral N>
  void operator()(std::integral_constant<Integral, N>&&) noexcept {
    std::cout << N << std::endl;
    ++results[petra::utilities::abs(N)];
    PETRA_ASSERT(results[petra::utilities::abs(N)] == 1);
  }

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
  run_test<USize>(petra::make_base_table<USize, void>(test<USize>{}));

  {
    constexpr auto test_with_error = [USize](auto&& i) noexcept {
      using T = decltype(i);
      if constexpr (petra::utilities::is_error_type<T>()) {
        return USize;
      } else {
        return std::decay_t<T>::value;
      }
    };
    auto table = petra::make_base_table<USize, std::size_t>(test_with_error);

#ifdef __clang__
    static_assert(noexcept(table(std::declval<std::size_t>())));
#endif

    run_test<USize>(table);
  }

  // Signed type
  {
    constexpr int SSize = 10;
    run_test<SSize>(petra::make_base_table<SSize, void>(test<SSize>{}));
  }

  return 0;
}
