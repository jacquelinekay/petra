#include "petra/switch_table.hpp"
#include "petra/utilities/sequence.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

using TestSet = std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) {
    std::cout << N << std::endl;
    constexpr std::size_t Index = petra::map_to_index<N>(TestSet{});
    PETRA_ASSERT(Index < TestSet::size());
    ++results[Index];
    PETRA_ASSERT(results[Index] == 1);
  }

  std::array<std::size_t, TestSet::size()> results = {{0}};
};

template<typename T, typename S>
void run_test(S&& table) {
  return run_test(T{}, table);
}

template<typename S, size_t... I>
void run_test(std::index_sequence<I...>, S&& table) {
  (table(I),...);
}

int main() {
  {
    run_test<TestSet>(petra::make_switch_table(test{}, TestSet{}));
  }

  {
    constexpr auto test_with_error = [](auto&& i) {
      return std::decay_t<decltype(i)>::value;
    };
    auto table = petra::make_switch_table(test_with_error, TestSet{}, TestSet::size());
    // run_test(table);
    // Try with an integer not in the set
    PETRA_ASSERT(table(33) == TestSet::size());
  }
  return 0;
}

