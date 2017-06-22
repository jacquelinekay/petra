// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/switch_table.hpp"
#include "petra/utilities/sequence.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/for_each.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <boost/hana/tuple.hpp>

namespace hana = boost::hana;
#endif  // PETRA_ENABLE_CPP14

using TestSet =
    std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) noexcept {
    std::cout << N << std::endl;
#ifdef PETRA_ENABLE_CPP14
    constexpr std::size_t Index = petra::map_to_index<std::size_t, N>(TestSet{});
#else
    constexpr std::size_t Index = petra::map_to_index<N>(TestSet{});
#endif
    PETRA_ASSERT(Index < TestSet::size());
    ++results[Index];
    PETRA_ASSERT(results[Index] == 1);
  }

  void operator()(petra::InvalidInputError&&) noexcept { PETRA_ASSERT(false); }

  std::array<std::size_t, TestSet::size()> results = {{0}};
};

template<typename S, size_t... I>
void run_test(std::index_sequence<I...>, S&& table) {
#ifdef PETRA_ENABLE_CPP14
  hana::for_each(hana::make_tuple(I...),
      [&table](auto i) {
        table(i);
      });
#else
  static_assert(noexcept(table(std::declval<std::size_t>())),
      "Noexcept correctness test failed!");

  (table(I), ...);
#endif  // PETRA_ENABLE_CPP14
}

int main() {
  { run_test(TestSet{}, petra::make_switch_table(test{}, TestSet{})); }

  {
#ifdef PETRA_ENABLE_CPP14
    auto test_with_error = hana::overload_linearly(
        [](petra::InvalidInputError&&) noexcept {
          return TestSet::size();
        },
        [](auto&& i) noexcept {
          return std::decay_t<decltype(i)>::value;
        });
#else
    constexpr auto test_with_error = [](auto&& i) noexcept {
      if constexpr (petra::utilities::is_error_type<decltype(i)>()) {
        return TestSet::size();
      } else {
        return std::decay_t<decltype(i)>::value;
      }
    };
#endif  // PETRA_ENABLE_CPP14
    auto table = petra::make_switch_table(test_with_error, TestSet{});
    // Try with an integer not in the set
    PETRA_ASSERT(table(33) == TestSet::size());
    run_test(TestSet{}, std::move(table));
  }

  {
#ifdef PETRA_ENABLE_CPP14
    auto test_with_exception = hana::overload_linearly(
        [](petra::InvalidInputError&&) {
          throw std::runtime_error("Detected invalid input.");
          return TestSet::size();
        },
        [](auto&& i) {
          return std::decay_t<decltype(i)>::value;
        });
#else
    auto test_with_exception = [](auto&& i) {
      if constexpr (petra::utilities::is_error_type<decltype(i)>()) {
        throw std::runtime_error("Detected invalid input.");
        return TestSet::size();
      } else {
        return std::decay_t<decltype(i)>::value;
      }
    };
#endif  // PETRA_ENABLE_CPP14
    auto table = petra::make_switch_table(test_with_exception, TestSet{});

#ifndef PETRA_ENABLE_CPP14
    static_assert(!noexcept(table(std::declval<std::size_t>())),
        "Noexcept correctness test failed for throwing function.");
#else
    (void)table;
#endif  // PETRA_ENABLE_CPP14
  }
  return 0;
}
