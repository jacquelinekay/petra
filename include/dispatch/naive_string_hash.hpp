#pragma once

#include <tuple>
#include <utility>
#include "dispatch/string_literal.hpp"

#include <iostream>

namespace sl = jk::string_literal;

/* Naive
 * With a tuple you can retrieve a constexpr integer too
 * but this is O(n) in the size of the set--can we do better?
 * TODO: broken
 * */
template<template<size_t> typename F, typename StringSet, size_t ...I>
constexpr auto make_string_map_naive_helper(StringSet&& string_set, std::index_sequence<I...>&&) {
  return [string_set = std::move(string_set)](const char* input) {
    ([&string_set, &input]() {
      if (sl::equal(std::get<I>(string_set), input)) {
        F<I>{}();
      }
    }(), ...);
  };
}

template<template<size_t> typename F, typename... Strings>
constexpr auto make_naive_string_hash(Strings&&... strings) {
  return make_string_map_naive_helper<F>(std::make_tuple(strings...),
      std::make_index_sequence<sizeof...(Strings)>{});
}
