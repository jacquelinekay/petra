// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/utilities.hpp"
#include "petra/utilities/sequence.hpp"

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/if.hpp>
#include <experimental/tuple>
#endif  // PETRA_ENABLE_CPP14

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(a)                                 \
  callable(std::integral_constant<Integral, I>{}, std::forward<Args>(a)...)

// this allows silent failure without a callback when return type is void
#ifdef PETRA_ENABLE_CPP14
#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  using Result =                                                               \
      std::result_of_t<F(std::integral_constant<Integral, I>, Args...)>;       \
  boost::hana::if_(boost::hana::bool_c<Iterations < sizeof...(Sequence)>, \
    [this, &i, tuple_args = std::make_tuple(std::forward<Args>(args)...)]() { \
      switch (i) {                                                               \
      case I:                   \
          return std::experimental::apply([this](auto&&... a) { \
            return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(a);         \
          }, tuple_args); \
      default:                                                                 \
        constexpr std::size_t next = Iterations + 1;                           \
        return std::experimental::apply([this, &i](auto&&... a) { \
          return this->apply<static_cast<Integral>(access_sequence<next>(IndexSeq{})), next>( \
            i, a...); \
        }, tuple_args);                                   \
      }                                                                          \
    }, \
    [this](auto&&... args) { \
      return boost::hana::if_( \
        boost::hana::bool_c<ErrorCallback || !std::is_same<Result, void>{}>, \
        [this, tuple_args = std::make_tuple(std::forward<Args>(args)...)]() { \
          return std::experimental::apply([this](auto&&... a) { \
            return callable(InvalidInputError{}, std::forward<Args>(a)...);         \
          }, tuple_args); \
        } \
      ); \
    } \
  );
#else
#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  using Result =                                                               \
      std::result_of_t<F(std::integral_constant<Integral, I>, Args...)>;       \
  if constexpr (Iterations < sizeof...(Sequence)) {                            \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(args);                   \
      default:                                                                 \
        constexpr std::size_t next = Iterations + 1;                           \
        return this->apply<access_sequence<next>(IndexSeq{}), next>(           \
            i, std::forward<Args>(args)...);                                   \
    }                                                                          \
  } else if constexpr (ErrorCallback || !std::is_same<Result, void>{}) {       \
    return callable(InvalidInputError{}, std::forward<Args>(args)...);         \
  }
#endif  // PETRA_ENABLE_CPP14

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  template<typename F, typename IndexSeq,
           typename ErrorCallback = std::true_type>
  struct SwitchTable;

  template<typename F, typename Integral, Integral... Sequence,
           bool ErrorCallback>
  struct SwitchTable<F, std::integer_sequence<Integral, Sequence...>,
                     petra::bool_constant<ErrorCallback>> {
    constexpr SwitchTable(F&& f) : callable(f) {}

    using IndexSeq = std::integer_sequence<Integral, Sequence...>;

    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) noexcept(
        noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(args))) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<Integral I, std::size_t Iterations, typename... Args>
    constexpr auto apply(Integral i, Args&&... args) const
        noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(args))) {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(
            this->apply<access_sequence<0>(IndexSeq{}), 0>(
                i, std::forward<Args>(args)...));

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) const
        PETRA_NOEXCEPT_FUNCTION_BODY(
            this->apply<static_cast<Integral>(access_sequence<0>(IndexSeq{})), 0>(
                i, std::forward<Args>(args)...));

  private:
    F callable;
  };

  template<typename Integral, Integral... Sequence, typename F, bool E = true>
  static constexpr decltype(auto) make_switch_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, Sequence...>,
                      petra::bool_constant<E>>(std::forward<F>(f)));

  template<typename F, typename Integral, Integral... I, bool E = true>
  static constexpr decltype(auto)
  make_switch_table(F&& f, std::integer_sequence<Integral, I...>&&)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SwitchTable<F, std::integer_sequence<Integral, I...>,
                      petra::bool_constant<E>>(std::forward<F>(f)));

#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY

}  // namespace petra
