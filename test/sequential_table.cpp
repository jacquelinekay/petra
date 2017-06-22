// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/sequential_table.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/functional/overload_linearly.hpp>

namespace hana = boost::hana;
#endif  // PETRA_ENABLE_CPP14

template<typename Integral, Integral Size>
struct test {
  template<Integral N>
  void operator()(std::integral_constant<Integral, N>&&) {
    std::cout << N << std::endl;
    ++results[petra::utilities::abs(N)];
    PETRA_ASSERT(results[petra::utilities::abs(N)] == 1);
  }
  void operator()(petra::InvalidInputError&&) noexcept { PETRA_ASSERT(false); }

  std::array<std::size_t, petra::utilities::abs(Size)> results = {{0}};
};

template<typename Integral, Integral Size, typename S>
void run_test(S&& table) {
  for (Integral i = 0; i < Size; ++i) { table(i); }
}

int main() {
  constexpr std::size_t USize = 10ul;
  {
#ifdef PETRA_ENABLE_CPP14
    run_test<std::size_t, USize>(
        petra::make_sequential_table<std::size_t, USize>(test<std::size_t, USize>{}));
#else
    run_test<std::size_t, USize>(
        petra::make_sequential_table<USize>(test<std::size_t, USize>{}));
#endif  // PETRA_ENABLE_CPP14
  }

  {
#ifdef PETRA_ENABLE_CPP14
    auto test_with_error = hana::overload_linearly(
        [](petra::InvalidInputError&&) noexcept {
          return USize;
        },
        [](auto&& i) noexcept {
          return std::decay_t<decltype(i)>::value;
        });
#else
    constexpr auto test_with_error = [USize](auto&& i) noexcept {
      using T = decltype(i);
      if constexpr (petra::utilities::is_error_type<T>()) {
        return USize;
      } else {
        return std::decay_t<T>::value;
      }
    };
#endif  // PETRA_ENABLE_CPP14
    auto table = petra::make_sequential_table<std::size_t, USize>(test_with_error);

    static_assert(noexcept(table(std::declval<std::size_t>())),
        "Noexcept correctness test failed for sequential_table.");

    run_test<std::size_t, USize>(table);
    // Try with an integer not in the set
    PETRA_ASSERT(table(20) == USize);
  }

  {
#ifdef PETRA_ENABLE_CPP14
    auto test_with_exception = hana::overload_linearly(
        [](petra::InvalidInputError&&) {
          throw std::runtime_error("Invalid input detected");
          return USize;
        },
        [](auto&& i) {
          return std::decay_t<decltype(i)>::value;
        });
#else
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
#endif  // PETRA_ENABLE_CPP14
    auto table = petra::make_sequential_table<std::size_t, USize>(test_with_exception);
    static_assert(!noexcept(table(std::declval<std::size_t>())),
        "Noexcept test failed for throwing callback in sequential_table.");
  }

  // Unsigned type
  {
    constexpr int SSize = 10;
    run_test<int, SSize>(petra::make_sequential_table<int, SSize>(test<int, SSize>{}));
  }

  return 0;
}
