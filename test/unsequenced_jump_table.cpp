#include <array>
#include "dispatch/unsequenced_jump_table.hpp"

using TestSet = std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;

template<typename F, size_t ...Sequence>
auto make_unsequenced_jump_table(F&& f, std::index_sequence<Sequence...>&&) {
  return unsequenced_jump_table<F, Sequence...>{f};
}

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) {
    ++results[N];
    assert(results[N] == 1);
  }
  std::array<int, TestSet::size()> results = {{0}};
};

template<typename Table, size_t... Sequence>
void run_tests(Table&& t, std::index_sequence<Sequence...>&&) {
  // This line segfaults clang
  (t(Sequence), ...);
}

int main() {
  run_tests(make_unsequenced_jump_table(test{}, TestSet{}), TestSet{});

  return 0;
}
