#pragma once

#include <cassert>

#include "dispatch/utilities.hpp"
/* Disadvantages:
 * Recursive template instantiation limit.
 * */

// TODO: use enable_if to merge these structs
template<typename F, std::size_t ...Sequence>
struct mutable_recursive_switch_table {
  F callable;

  using IndexSequence = std::index_sequence<Sequence...>;
  template<std::size_t I, std::size_t Iterations, typename ...Args>
  constexpr auto apply(std::size_t i, Args&&... args) {
    using Result = std::result_of_t<F(std::integral_constant<std::size_t, I>, Args...)>;
    if constexpr (Iterations < sizeof...(Sequence)) {
      switch(i) {
        case I:
          return callable(std::integral_constant<std::size_t, I>{}, std::forward<Args>(args)...);
        default:
          constexpr std::size_t next = Iterations + 1;
          return apply<access_sequence<next>(IndexSequence{}), next>(i, std::forward<Args>(args)...);
      }
    } else if constexpr (!std::is_same<Result, void>{}) {
      return Result{};
    }
  }

  template<typename ...Args>
  constexpr auto operator()(std::size_t i, Args&&... args) {
    return apply<access_sequence<0>(IndexSequence{}), 0>(i, std::forward<Args>(args)...);
  }
};

template<typename F, std::size_t ...Sequence>
struct recursive_switch_table {
  const F callable;

  using IndexSequence = std::index_sequence<Sequence...>;
  template<std::size_t I, std::size_t Iterations, typename ...Args>
  constexpr decltype(auto) apply(std::size_t i, Args&&... args) const {
    using Result = std::result_of_t<F(std::integral_constant<std::size_t, I>, Args...)>;
    if constexpr (Iterations < sizeof...(Sequence)) {
      switch(i) {
        case I:
          return callable(std::integral_constant<std::size_t, I>{}, std::forward<Args>(args)...);
        default:
          constexpr std::size_t next = Iterations + 1;
          return apply<access_sequence<next>(IndexSequence{}), next>(i, std::forward<Args>(args)...);
      }
    } else if constexpr (!std::is_same<Result, void>{}) {
      return Result{};
    }
  }

  template<typename ...Args>
  constexpr auto operator()(std::size_t i, Args&&... args) const {
    return apply<access_sequence<0>(IndexSequence{}), 0>(i, std::forward<Args>(args)...);
  }
};


template< std::size_t ...Sequence, typename F>
static constexpr auto make_mutable_recursive_switch_table(F&& f) {
  return mutable_recursive_switch_table<F, Sequence...>{f};
}

template< std::size_t ...Sequence, typename F>
static constexpr auto make_recursive_switch_table(F&& f) {
  return recursive_switch_table<F, Sequence...>{f};
}

template< std::size_t ...Sequence, typename F>
static constexpr auto make_recursive_switch_table(F&& f, std::index_sequence<Sequence...>) {
  return recursive_switch_table<F, Sequence...>{f};
}
