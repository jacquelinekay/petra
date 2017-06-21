// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>

#include "petra/concepts.hpp"
#include "petra/utilities.hpp"

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()                                 \
  callable(std::integral_constant<Integral, I>{}, std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY() \
  switch (i) {                                                               \
    case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
    default: return apply(std::integral_constant<Integral, I + 1>{}, i, std::forward<Args>(args)...);            \
  }                                                                          \

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

#ifndef PETRA_ENABLE_CPP14
  // Utility for error handling
  template<typename T, auto Size>
  static constexpr bool invalid_input() noexcept {
    if constexpr (Constant<T>()) {
      if constexpr (std::is_integral<decltype(T::value)>{}) {
        return T::value >= Size;
      }
    }
    return true;
  }

#endif  // PETRA_ENABLE_CPP14

  // A specialization of SwitchTable for a positive sequential set of integers
  template<typename F, typename Integral, Integral N>
  struct SequentialTable {
    static_assert(N > 0,
        "Sorry, SequentialTable only supports positive sequences.");

    template<Integral I>
    using int_c = std::integral_constant<Integral, I>;

    F callable;

    constexpr SequentialTable(F&& f) noexcept : callable(f) {}

    template<typename Iterations, typename... Args>
    constexpr auto apply(Iterations&&, Integral, Args&&...);

    template<typename... Args>
    constexpr auto apply(int_c<N>&&, Integral, Args&&... args)
    PETRA_NOEXCEPT_FUNCTION_BODY(callable(InvalidInputError{}, std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto apply(int_c<N>&&, Integral, Args&&... args) const
    PETRA_NOEXCEPT_FUNCTION_BODY(callable(InvalidInputError{}, std::forward<Args>(args)...));

    template<Integral I, typename... Args>
    constexpr auto apply(int_c<I>&&, Integral i, Args&&... args) noexcept(
        noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<Integral I, typename... Args>
    constexpr auto apply(int_c<I>&&, Integral i, Args&&... args) const
        noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(apply(int_c<0>{}, i, std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(apply(int_c<0>{}, i, std::forward<Args>(args)...));
  };

#ifndef PETRA_ENABLE_CPP14
  template<auto N, typename F>
  constexpr decltype(auto) make_sequential_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, decltype(N), N>{std::forward<F>(f)});
#endif  //PETRA_ENABLE_CPP14 

  template<typename Integral, Integral N, typename F>
  constexpr decltype(auto) make_sequential_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, Integral, N>{std::forward<F>(f)});

#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY

}  // namespace petra
