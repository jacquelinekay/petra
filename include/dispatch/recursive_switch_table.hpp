#pragma once

#include <cassert>

#include "dispatch/utilities.hpp"

template<template<size_t> typename F, size_t ...Sequence>
struct recursive_switch_table {
  using IndexSequence = std::index_sequence<Sequence...>;
  // Problem: Recursive template instantiation
  template<size_t I, size_t Iterations>
  static auto apply(unsigned i) {
    if constexpr (Iterations >= sizeof...(Sequence)) {
      // error condition
      assert(false);
    } else {
      switch(i) {
        case I:
          return F<I>{}();
        default:
          constexpr unsigned next = Iterations + 1;
          // Try the next thing in the sequence
          return apply<access_sequence<next>(IndexSequence{}), next>(i);
      }
    }
  }

  void operator()(unsigned i) const {
    return apply<access_sequence<0>(IndexSequence{}), 0>(i);
  }
};


