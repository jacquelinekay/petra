#pragma once

#include "dispatch/recursive_switch_table.hpp"
// #include "dispatch/unsequenced_jump_table.hpp"

template<template<typename...> class Hash, typename F, typename ...Strings>
struct string_dispatch {
  string_dispatch(F&& callable) : table{callable} {
  }

  mutable_recursive_switch_table<F, Hash<Strings...>::hash(Strings{})...> table;

  static constexpr auto string_hash = Hash<Strings...>{};

  auto operator()(const char* value) {
    return table(string_hash(value));
  }
};

template<template<typename...> class Hash, typename F, typename ...Strings>
auto make_string_dispatch(F&& f, Strings&&...) {
  return string_dispatch<Hash, F, Strings...>(std::forward<F>(f));
}
