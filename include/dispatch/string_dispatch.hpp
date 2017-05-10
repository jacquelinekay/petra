#pragma once

#include "dispatch/recursive_switch_table.hpp"
// #include "dispatch/unsequenced_jump_table.hpp"

template<template<typename...> class Hash, template<size_t> typename F, typename ...Strings>
struct string_dispatch {
  static constexpr auto string_hash = Hash<Strings...>{};
  static constexpr recursive_switch_table<F, Hash<Strings...>::hash(Strings{})...> table;

  auto operator()(const char* value) const {
    return table(string_hash(value));
  }
};

template<template<typename...> class Hash, template<size_t> typename F, typename ...Strings>
constexpr auto make_string_dispatch(Strings&&...) {
  return string_dispatch<Hash, F, Strings...>{};
}
