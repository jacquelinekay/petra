#include <array>
#include "dispatch/unsequenced_jump_table.hpp"

using TestSet = std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;
static std::array<int, TestSet::size()> results = {0};

template<template<size_t> typename F, size_t ...Sequence>
constexpr auto make_unsequenced_jump_table(std::index_sequence<Sequence...>&&) {
  return unsequenced_jump_table<F, Sequence...>{};
}

template<size_t N>
struct test {
  void operator()() const {
    std::cout << N << "\n";
    ++results[N];
  }
};

template<typename Table, size_t... Sequence>
void run_tests(Table&& t, std::index_sequence<Sequence...>&&) {
  (t(Sequence), ...);
}

int main() {
  constexpr auto unsequenced_jump_table = make_unsequenced_jump_table<test>(TestSet{});

  run_tests(unsequenced_jump_table, TestSet{});

  for (const auto result : results) {
    assert(result == 1);
  }

  return 0;
}
