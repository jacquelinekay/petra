#pragma once

#include "dispatch/recursive_switch_table.hpp"

namespace dispatch {
// TODO: just merge with simple string hash

template<template<typename...> class Hash, typename F, typename ...Strings>
struct string_dispatch {
  string_dispatch(F&& callable) : table{callable} { }

  recursive_switch_table<F, Hash<Strings...>::hash(Strings{})...> table;

  static constexpr auto string_hash = Hash<Strings...>{};

  auto operator()(const char* value) {
    return table(string_hash(value));
  }
};

template<template<typename...> class Hash, typename F, typename ...Strings>
auto make_string_dispatch(F&& f, Strings&&...) {
  return string_dispatch<Hash, F, Strings...>(std::forward<F>(f));
}

}  // namespace dispatch
