#pragma once

#include <cassert>

#include "dispatch/utilities.hpp"
#include "dispatch/utilities/sequence.hpp"
/* Disadvantages:
 * Recursive template instantiation limit.
 * */

namespace dispatch {

#define DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY() \
  using Result = std::result_of_t<F( \
      std::integral_constant<std::size_t, I>, Args...)>; \
  if constexpr (Iterations < sizeof...(Sequence)) { \
    switch(i) { \
      case I: \
        return callable( \
            std::integral_constant<std::size_t, I>{}, \
            std::forward<Args>(args)...); \
      default: \
        constexpr std::size_t next = Iterations + 1; \
        return apply<access_sequence<next>( \
            IndexSequence{}), next>(i, std::forward<Args>(args)...); \
    } \
  } else if constexpr (!std::is_same<Result, void>{}) { \
    return Result{}; \
  } \


template<typename F, std::size_t ...Sequence>
struct recursive_switch_table {
  F callable;

  using IndexSequence = std::index_sequence<Sequence...>;
  template<std::size_t I, std::size_t Iterations, typename ...Args>
  constexpr auto apply(std::size_t i, Args&&... args) {
    DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
  }

  template<std::size_t I, std::size_t Iterations, typename ...Args>
  constexpr auto apply(std::size_t i, Args&&... args) const {
    DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
  }

  template<typename ...Args>
  constexpr auto operator()(std::size_t i, Args&&... args) {
    return apply<access_sequence<0>(IndexSequence{}), 0>(
        i, std::forward<Args>(args)...);
  }

  template<typename ...Args>
  constexpr auto operator()(std::size_t i, Args&&... args) const {
    return apply<access_sequence<0>(IndexSequence{}), 0>(
        i, std::forward<Args>(args)...);
  }
};

template< std::size_t ...Sequence, typename F>
static constexpr auto make_recursive_switch_table(F&& f) {
  return recursive_switch_table<F, Sequence...>{f};
}

template< std::size_t ...Sequence, typename F>
static constexpr auto make_recursive_switch_table(
    F&& f, std::index_sequence<Sequence...>) {
  return recursive_switch_table<F, Sequence...>{f};
}

#undef DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY

}  // namespace dispatch
