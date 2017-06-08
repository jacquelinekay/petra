// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

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
        return this->apply<access_sequence<next>(IndexSeq{}), next>(           \
            i, std::forward<Args>(args)...);                                   \
    }                                                                          \
  } else if constexpr (!std::is_same<Result, void>{}) {                        \
    (void)i;                                                                   \
    if constexpr (!std::is_same<ErrorType, void*>{}) {                         \
      return error_value;                                                      \
    } else {                                                                   \
      return static_cast<Result>(error_value);                                 \
    }                                                                          \
  }

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  template<typename F, typename IndexSeq, typename ErrorType = void*>
  struct SwitchTable;

  template<typename F, typename Integral, Integral... Sequence,
           typename ErrorType>
  struct SwitchTable<F, std::integer_sequence<Integral, Sequence...>,
                     ErrorType> {
    static constexpr auto get_error_val = []() {
      if constexpr (std::is_same<ErrorType, void*>{}) {
        return nullptr;
      } else {
        // TODO: Handle non-default-constructible case
        return ErrorType{};
      }
    };

    constexpr SwitchTable(F&& f) : callable(f), error_value(get_error_val()) {}

    constexpr SwitchTable(F&& f, ErrorType&& e) : callable(f), error_value(e) {}

    F callable;
    const ErrorType error_value;

    using IndexSeq = std::integer_sequence<Integral, Sequence...>;
    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) noexcept(
        noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) const
        noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) {
      // PETRA_NOEXCEPT_FUNCTION_BODY(this->apply<access_sequence<0>(IndexSeq{}),
      // 0>(
      //    i, std::forward<Args>(args)...));
      return this->apply<access_sequence<0>(IndexSeq{}), 0>(
          i, std::forward<Args>(args)...);
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const {
      // PETRA_NOEXCEPT_FUNCTION_BODY(this->apply<access_sequence<0>(IndexSeq{}),
      // 0>(
      //      i, std::forward<Args>(args)...))
      return this->apply<access_sequence<0>(IndexSeq{}), 0>(
          i, std::forward<Args>(args)...);
    }
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

  template<typename Integral, Integral... Sequence, typename F, typename E>
  static constexpr decltype(auto) make_switch_table(F&& f, E&& e)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, Sequence...>, E>(
              std::forward<F>(f), std::forward<E>(e)));

  template<typename F, typename IndexSeq, typename E>
  static constexpr decltype(auto) make_switch_table(F&& f, IndexSeq, E&& e)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, IndexSeq, E>(std::forward<F>(f), std::forward<E>(e)));

#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY

}  // namespace petra
