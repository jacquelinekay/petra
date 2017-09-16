// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/detail/macros.hpp"
#include "petra/utilities.hpp"
#include "petra/utilities/sequence.hpp"

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()                                 \
  callable(std::integral_constant<Integral, I>{}, std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  using Result =                                                               \
      std::result_of_t<F(std::integral_constant<Integral, I>, Args...)>;       \
  if constexpr (Iterations < sizeof...(Sequence)) {                            \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
      default:                                                                 \
        constexpr std::size_t next = Iterations + 1;                           \
        return apply<access_sequence<next>(IndexSeq{}), next>(                 \
            i, std::forward<Args>(args)...);                                   \
    }                                                                          \
  } else if constexpr (ErrorCallback || !std::is_same<Result, void>{}) {       \
    (void)i;                                                                   \
    return callable(InvalidInputError{}, std::forward<Args>(args)...);         \
  }

  template<typename F, typename IndexSeq,
           typename ErrorCallback = std::true_type>
  struct SwitchTable;

  template<typename F, typename Integral, Integral... Sequence,
           bool ErrorCallback>
  struct SwitchTable<F, std::integer_sequence<Integral, Sequence...>,
                     std::bool_constant<ErrorCallback>> {
    constexpr SwitchTable(F&& f) : callable(f) {}

    using IndexSeq = std::integer_sequence<Integral, Sequence...>;
    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args)
        PETRA_NOEXCEPT_CHECK(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_CHECK((PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(apply<access_sequence<0>(IndexSeq{}), 0>(
            i, std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(apply<access_sequence<0>(IndexSeq{}), 0>(
            i, std::forward<Args>(args)...));

  private:
    F callable;
  };

  template<typename Integral, Integral... Sequence, typename F, bool E = true>
  static constexpr decltype(auto) make_switch_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, Sequence...>,
                      std::bool_constant<E>>(std::forward<F>(f)));

  template<typename F, typename Integral, Integral... I, bool E = true>
  static constexpr decltype(auto)
  make_switch_table(F&& f, std::integer_sequence<Integral, I...>&&)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, I...>,
                      std::bool_constant<E>>(std::forward<F>(f)));

#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY

}  // namespace petra
