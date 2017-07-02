// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/detail/macros.hpp"
#include "petra/utilities.hpp"
#include "petra/utilities/sequence.hpp"

#ifdef PETRA_ENABLE_CPP14
#include <experimental/tuple>
#endif  // PETRA_ENABLE_CPP14

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()                                 \
  callable(std::integral_constant<Integral,                                    \
                                  access_sequence<Iterations>(IndexSeq{})>{},  \
           std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  constexpr auto I = access_sequence<Iterations>(IndexSeq{});                  \
  switch (i) {                                                                 \
    case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                     \
    default:                                                                   \
      return this->apply(                                                      \
          std::integral_constant<std::size_t, Iterations + 1>{}, i,            \
          std::forward<Args>(args)...);                                        \
  }

  template<typename F, typename IndexSeq>
  struct SwitchTable;

  template<typename F, typename Integral, Integral... Sequence>
  struct SwitchTable<F, std::integer_sequence<Integral, Sequence...>> {
    constexpr SwitchTable(F&& f) : callable(f) {}

    using IndexSeq = std::integer_sequence<Integral, Sequence...>;

    template<std::size_t I>
    using size_c = std::integral_constant<std::size_t, I>;

    template<typename Iterations, typename... Args>
    constexpr auto apply(Iterations&&, Integral, Args&&...);

    template<typename... Args>
    constexpr auto apply(size_c<sizeof...(Sequence)>&&, Integral,
                         Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(callable(InvalidInputError{},
                                              std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto apply(size_c<sizeof...(Sequence)>&&, Integral,
                         Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(callable(InvalidInputError{},
                                              std::forward<Args>(args)...));

    template<std::size_t Iterations, typename... Args>
    constexpr auto
    apply(size_c<Iterations>&&, Integral i, Args&&... args)
#ifdef __clang__
    noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()))
#endif  // __clang__
    {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<std::size_t Iterations, typename... Args>
    constexpr auto apply(size_c<Iterations>&&, Integral i, Args&&... args) const
#ifdef __clang__
    noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()))
#endif  // __clang__
    {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(apply(size_c<0>{}, i,
                                                 std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(apply(size_c<0>{}, i,
                                                 std::forward<Args>(args)...));

    F callable;
  };

  template<typename Integral, Integral... Sequence, typename F>
  static constexpr decltype(auto) make_switch_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, Sequence...>>(
              std::forward<F>(f)));

  template<typename F, typename Integral, Integral... I>
  static constexpr decltype(auto)
  make_switch_table(F&& f, std::integer_sequence<Integral, I...>&&)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, I...>>(
              std::forward<F>(f)));

#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS

}  // namespace petra
