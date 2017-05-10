#pragma once

#include <array>
#include <utility>

// TODO: take out constexpr lambdas if C++14
/* Disadvantages:
 * Limited to sequential integer lookup.
 * Return type of the callbacks must be uniform.
 * */

template<template<size_t> typename F, size_t ...I>
constexpr auto make_table_helper(std::index_sequence<I...>&&) {
  using return_type = std::result_of_t<F<0>()>;
  static_assert((std::is_same<return_type, std::result_of_t<F<I>()>>{} && ...));

  return std::array<return_type(*)(), sizeof...(I)> {
    +[]() {
      return F<I>{}();
    }...
  };
}

template<template<size_t> typename F, size_t N>
constexpr auto make_jump_table() {
  return make_table_helper<F>(std::make_index_sequence<N>{});
}
