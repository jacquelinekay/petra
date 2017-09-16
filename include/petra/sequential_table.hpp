// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>

#include "petra/concepts.hpp"
#include "petra/detail/macros.hpp"
#include "petra/utilities.hpp"

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()                                 \
  callable(std::integral_constant<Integral, I>{}, std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  using Result =                                                               \
      std::result_of_t<F(std::integral_constant<Integral, I>, Args...)>;       \
  if constexpr (std::is_signed<Integral>{} && N < 0 && I > N) {                \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
      default: return apply<I - 1>(i, std::forward<Args>(args)...);            \
    }                                                                          \
  } else if constexpr ((std::is_unsigned<Integral>{} || N > 0) && I < N) {     \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
      default: return apply<I + 1>(i, std::forward<Args>(args)...);            \
    }                                                                          \
  } else if constexpr (ErrorCallback{} || !std::is_same<Result, void>{}) {     \
    (void)i;                                                                   \
    return callable(InvalidInputError{}, std::forward<Args>(args)...);         \
  }

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

  // A specialization of SwitchTable for a sequential set of integers
  template<typename F, auto N, typename ErrorCallback = std::true_type>
  struct SequentialTable {
    using Integral = decltype(N);
    F callable;

    constexpr SequentialTable(F&& f) noexcept : callable(f) {}

    template<Integral I, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) noexcept(
        noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<Integral I, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) const
        noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i, std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i, std::forward<Args>(args)...));
  };

  template<auto N, typename F, bool E = true>
  constexpr decltype(auto) make_sequential_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N, std::bool_constant<E>>{
          std::forward<F>(f)});

#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY

}  // namespace petra
