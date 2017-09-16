// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/switch_table.hpp"
#include "petra/utilities/sequence.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

using TestSet =
    std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) noexcept {
    std::cout << N << std::endl;
    constexpr std::size_t Index = petra::map_to_index<N>(TestSet{});
    PETRA_ASSERT(Index < TestSet::size());
    ++results[Index];
    PETRA_ASSERT(results[Index] == 1);
  }

  void operator()(petra::InvalidInputError&&) noexcept { PETRA_ASSERT(false); }

  std::array<std::size_t, TestSet::size()> results = {{0}};
};

template<typename T, typename S>
void run_test(S&& table) {
  return run_test(T{}, table);
}

template<typename S, size_t... I>
void run_test(std::index_sequence<I...>, S&& table) {
#ifdef __clang__
  static_assert(noexcept(table(std::declval<std::size_t>())));
#endif
  (table(I), ...);
}

int main() {
  { run_test<TestSet>(petra::make_switch_table(test{}, TestSet{})); }

  {
    constexpr auto test_with_error = [](auto&& i) noexcept {
      if constexpr (petra::utilities::is_error_type<decltype(i)>()) {
        return TestSet::size();
      } else {
        return std::decay_t<decltype(i)>::value;
      }
    };
    auto table = petra::make_switch_table(test_with_error, TestSet{});
    // run_test(table);
    // Try with an integer not in the set
    PETRA_ASSERT(table(33) == TestSet::size());
  }

  {
    auto test_with_exception = [](auto&& i) {
      if constexpr (petra::utilities::is_error_type<decltype(i)>()) {
        throw std::runtime_error("Detected invalid input.");
        return TestSet::size();
      } else {
        return std::decay_t<decltype(i)>::value;
      }
    };
    auto table = petra::make_switch_table(test_with_exception, TestSet{});

    static_assert(!noexcept(table(std::declval<std::size_t>())));
  }
  return 0;
}
