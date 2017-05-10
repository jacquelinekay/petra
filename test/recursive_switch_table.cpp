#include "dispatch/recursive_switch_table.hpp"

#include <array>
#include <iostream>

// TODO: meta fuzz tests would be cool

using TestSet = std::index_sequence<400, 32, 1, 99999, 1337, 42, 123456789, 0, 2, 2048>;
static std::array<int, TestSet::size()> results = {0};

template<size_t N>
struct test {
  void operator()() const {
    std::cout << N << std::endl;
    ++results[N];
  }
};

template<template<size_t> typename F, typename T>
struct switch_table_wrapper;

template<template<size_t> typename F, size_t ...I>
struct switch_table_wrapper<F, std::index_sequence<I...>> : recursive_switch_table<F, I...> {};

template<typename T, typename S>
void run_test(S&& table) {
  return run_test(T{}, table);
}

template<typename S, size_t... I>
void run_test(std::index_sequence<I...>, S&& table) {
  (table(I),...);
}

int main() {
  run_test<TestSet>(switch_table_wrapper<test, TestSet>{});

  for (const auto result : results) {
    assert(result == 1);
  }
}

