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

template<typename F, std::size_t ...Sequence>
struct unsequenced_jump_table {
  F callable;

  template<std::size_t ...I>
  void helper(std::size_t i, std::index_sequence<I...>&&) {
    ([&i, this]() {
      if (i == Sequence) {
        callable(std::integral_constant<std::size_t, I>{});
      }
    }(), ...);
  }

  void operator()(std::size_t i) {
    helper(i, std::make_index_sequence<sizeof...(Sequence)>{});
  }
};
