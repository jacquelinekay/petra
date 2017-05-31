#include "dispatch/sequential_table.hpp"

#include "utilities.hpp"

#include <array>
#include <iostream>

constexpr std::size_t Size = 10;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>&&) {
    std::cout << N << std::endl;
    ++results[N];
    DISPATCH_ASSERT(results[N] == 1);
  }

  std::array<std::size_t, Size> results = {{0}};
};


template<typename S>
void run_test(S&& table) {
  for (std::size_t i = 0; i < Size; ++i) {
    table(i);
  }
}

int main() {
  run_test(dispatch::make_sequential_table<Size>(test{}));
  return 0;
}

