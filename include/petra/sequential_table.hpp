#pragma once

#include <utility>

namespace petra {

#define PETRA_RECURSIVE_SWITCH_TABLE_RETURNS() \
  callable(std::integral_constant<std::size_t, I>{}, \
           std::forward<Args>(args)...)

#define PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY() \
  using Result = std::result_of_t<F( \
    std::integral_constant<std::size_t, I>, Args...)>; \
  if constexpr (I < N) { \
    switch(i) { \
      case I: \
        return PETRA_RECURSIVE_SWITCH_TABLE_RETURNS(); \
      default: \
        return apply<I + 1>(i, std::forward<Args>(args)...); \
    } \
  } else if constexpr (!std::is_same<Result, void>{}) { \
    if constexpr (!std::is_same<ErrorType, void*>{}) { \
      return error_value; \
    } else { \
      return static_cast<Result>(error_value); \
    } \
  } \


#define PETRA_NOEXCEPT_FUNCTION_BODY(...) \
  noexcept(noexcept(__VA_ARGS__)) { \
    return __VA_ARGS__; \
  } \

  // A specializaion of SwitchTable for a sequential set of integers
  template<typename F, std::size_t N, typename ErrorType = void*>
  struct SequentialTable {
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

    constexpr SequentialTable(F&& f) : callable(f), error_value(get_error_val()) {
    }

    constexpr SequentialTable(F&& f, ErrorType&& e) : callable(f), error_value(e) {
    }

    template<std::size_t I, typename ...Args>
    constexpr auto apply(std::size_t i, Args&&... args)
    noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()))
    {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<std::size_t I, typename ...Args>
    constexpr auto apply(std::size_t i, Args&&... args) const
    noexcept(noexcept(PETRA_RECURSIVE_SWITCH_TABLE_RETURNS()))
    {
      PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY()
    }

    template<typename ...Args>
    constexpr auto operator()(std::size_t i, Args&&... args) const
    PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i, std::forward<Args>(args)...))

    template<typename ...Args>
    constexpr auto operator()(std::size_t i, Args&&... args)
    PETRA_NOEXCEPT_FUNCTION_BODY(apply<0>(i, std::forward<Args>(args)...))

  };

  template<std::size_t N, typename F>
  constexpr decltype(auto) make_sequential_table(F&& f)
  PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N>{std::forward<F>(f)});

  template<std::size_t N, typename F, typename ErrorType>
  constexpr decltype(auto) make_sequential_table(F&& f, ErrorType&& error_value)
  PETRA_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N, ErrorType>{
      std::forward<F>(f), std::forward<ErrorType>(error_value)});

#undef PETRA_RECURSIVE_SWITCH_TABLE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY
#undef PETRA_RECURSIVE_SWITCH_TABLE_APPLY_BODY

}  // namespace petra
