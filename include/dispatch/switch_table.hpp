#pragma once

#include "dispatch/utilities.hpp"
#include "dispatch/utilities/sequence.hpp"

namespace dispatch {

#define DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS() \
  callable(std::integral_constant<std::size_t, I>{}, \
           std::forward<Args>(args)...)

#define DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY() \
  using Result = std::result_of_t<F( \
    std::integral_constant<std::size_t, I>, Args...)>; \
  if constexpr (Iterations < sizeof...(Sequence)) { \
    switch(i) { \
      case I: \
        return DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS(); \
      default: \
        constexpr std::size_t next = Iterations + 1; \
        return apply<access_sequence<next>( \
            IndexSeq{}), next>(i, std::forward<Args>(args)...); \
    } \
  } else if constexpr (!std::is_same<Result, void>{}) { \
    return error_value; \
  } \

#define DISPATCH_NOEXCEPT_FUNCTION_BODY(...) \
  noexcept(noexcept(__VA_ARGS__)) { \
    return __VA_ARGS__; \
  } \

  template<typename F, typename IndexSeq, typename ErrorType = void*>
  struct SwitchTable;

  template<typename F, std::size_t ...Sequence, typename ErrorType>
  struct SwitchTable<F, std::index_sequence<Sequence...>, ErrorType> {
    static constexpr auto get_error_val = []() {
      if constexpr (std::is_same<ErrorType, void*>{}) {
        return nullptr;
      } else {
        // TODO: Handle non-default-constructible case
        return ErrorType{};
      }
    };

    constexpr SwitchTable(F&& f) : callable(f), error_value(get_error_val()) {
    }

    constexpr SwitchTable(F&& f, ErrorType&& e) : callable(f), error_value(e) {
    }

    F callable;
    const ErrorType error_value;

    using IndexSeq = std::index_sequence<Sequence...>;
    template<std::size_t I, std::size_t Iterations, typename ...Args>
    constexpr auto apply(std::size_t i, Args&&... args)
    noexcept(noexcept(DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<std::size_t I, std::size_t Iterations, typename ...Args>
    constexpr auto apply(std::size_t i, Args&&... args) const
    noexcept(noexcept(DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS())) {
      DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename ...Args>
    constexpr auto operator()(std::size_t i, Args&&... args)
    DISPATCH_NOEXCEPT_FUNCTION_BODY(
      apply<access_sequence<0>(IndexSeq{}), 0>(i, std::forward<Args>(args)...)
    );

    template<typename ...Args>
    constexpr auto operator()(std::size_t i, Args&&... args) const
    DISPATCH_NOEXCEPT_FUNCTION_BODY(
      apply<access_sequence<0>(IndexSeq{}), 0>(i, std::forward<Args>(args)...)
    );
  };

  template<std::size_t ...Sequence, typename F>
  static constexpr decltype(auto) make_switch_table(F&& f)
  DISPATCH_NOEXCEPT_FUNCTION_BODY(
    SwitchTable<F, std::index_sequence<Sequence...>>(std::forward<F>(f))
  )

  template<typename F, typename IndexSeq>
  static constexpr decltype(auto) make_switch_table(F&& f, IndexSeq)
  DISPATCH_NOEXCEPT_FUNCTION_BODY(
    SwitchTable<F, IndexSeq>(std::forward<F>(f))
  )

  template<typename F, typename IndexSeq, typename ErrorType>
  static constexpr decltype(auto) make_switch_table(F&& f, IndexSeq, ErrorType&& error_value)
  DISPATCH_NOEXCEPT_FUNCTION_BODY(
    SwitchTable<F, IndexSeq>(std::forward<F>(f), std::forward<ErrorType>(error_value))
  )

#undef DISPATCH_RECURSIVE_SWITCH_TABLE_APPLY_BODY
#undef DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS
#undef DISPATCH_NOEXCEPT_FUNCTION_BODY

}  // namespace dispatch
