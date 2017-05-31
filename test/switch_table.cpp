#include "dispatch/switch_table.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

using TestSet = std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) {
    std::cout << N << std::endl;
    ++results[N];
    DISPATCH_ASSERT(results[N] == 1);
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
  run_test<TestSet>(dispatch::make_switch_table(test{}, TestSet{}));
  return 0;
}

