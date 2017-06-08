// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()                                 \
  callable(std::integral_constant<Integral, I>{},                           \
           std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()                              \
  using Result =                                                               \
      std::result_of_t<F(std::integral_constant<Integral, I>, Args...)>;       \
  if constexpr (std::is_signed<Integral>{} && N < 0 && I) {                  \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
      default: return apply<I - 1>(i, std::forward<Args>(args)...);            \
    }                                                                          \
  } else if constexpr (std::is_unsigned<Integral>{} && I < N) {              \
    switch (i) {                                                               \
      case I: return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS();                   \
      default: return apply<I + 1>(i, std::forward<Args>(args)...);            \
    }                                                                          \
  } else if constexpr (!std::is_same<Result, void>{}) {                        \
    (void)i;                                                                   \
    return static_cast<Result>(error_value);                                   \
  }

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  // A specialization of SwitchTable for a sequential set of integers
  template<typename F, auto N, typename ErrorType = void*>
  struct SequentialTable {
    using Integral = decltype(N);
    F callable;
    const ErrorType error_value;
    static constexpr auto get_error_val = []() {
      if constexpr (std::is_same<ErrorType, void*>{}) {
        return nullptr;
      } else {
        // TODO: Handle non-default-constructible case
        return ErrorType{};
      }
    };

    constexpr SequentialTable(F&& f)
        : callable(f), error_value(get_error_val()) {}

    constexpr SequentialTable(F&& f, ErrorType&& e)
        : callable(f), error_value(e) {}

    // TODO: Clean up
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
    constexpr auto operator()(Integral i, Args&&... args) const {
      //     PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i,
      //     std::forward<Args>(args)...));
      return this->apply<0>(i, std::forward<Args>(args)...);
    }

    template<typename... Args>
    constexpr auto operator()(Integral i, Args&&... args) {
      //     PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i,
      //     std::forward<Args>(args)...));
      return this->apply<0>(i, std::forward<Args>(args)...);
    }
  };

  template<auto N, typename F>
  constexpr decltype(auto) make_sequential_table(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N>{std::forward<F>(f)});

  template<auto N, typename F, typename ErrorType>
  constexpr decltype(auto) make_sequential_table(F&& f, ErrorType&& error_value)
      PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N, ErrorType>{
          std::forward<F>(f), std::forward<ErrorType>(error_value)});

#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY

}  // namespace petra
