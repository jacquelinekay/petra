#pragma once

#include <cassert>

#include "dispatch/utilities.hpp"
/* Disadvantages:
 * Recursive template instantiation limit.
 * */

template<template<size_t> typename F, size_t ...Sequence>
struct recursive_switch_table {
  using IndexSequence = std::index_sequence<Sequence...>;
  template<size_t I, size_t Iterations>
  static auto apply(unsigned i) {
    if constexpr (Iterations < sizeof...(Sequence)) {
      switch(i) {
        case I:
          return F<map_to_index<I, Sequence...>()>{}();
        default:
          constexpr unsigned next = Iterations + 1;
          return apply<access_sequence<next>(IndexSequence{}), next>(i);
      }
    }
  }

  void operator()(unsigned i) const {
    return apply<access_sequence<0>(IndexSequence{}), 0>(i);
  }
};
