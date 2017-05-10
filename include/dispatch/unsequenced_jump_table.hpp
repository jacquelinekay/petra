#pragma once

#include <cassert>
#include <utility>
#include <iostream>

/* Disadvantages:
 * Linear lookup complexity at runtime due to fold expression.
 * No return type from callbacks.
 *
 * Also, appears to be broken right now
 * */

// Given a set of any N integers at compile time,
// make a mapping to [0, N)
// map runtime integer in the original set to 
// TODO: make numeric type generic

template<template<size_t> typename F, size_t ...Sequence>
struct unsequenced_jump_table {
  template<size_t ...I>
  static auto helper(unsigned i, std::index_sequence<I...>&&) {
    ([&i]() {
      if (i == Sequence) {
        F<I>{}();
      }
    }(), ...);
  }

  auto operator()(unsigned i) const {
    return helper(i, std::make_index_sequence<sizeof...(Sequence)>{});
  }
};

