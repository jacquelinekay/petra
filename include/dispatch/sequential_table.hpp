#pragma once

#include <utility>

namespace dispatch {

#define DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS() \
  callable(std::integral_constant<std::size_t, I>{}, \
           std::forward<Args>(args)...)

#define DISPATCH_NOEXCEPT_FUNCTION_BODY(...) \
  noexcept(noexcept(__VA_ARGS__)) { \
    return __VA_ARGS__; \
  } \

  // Conceptually, an optimization of SwitchTable for a sequential set of integers
  template<typename F, std::size_t N>
  struct SequentialTable {
    F callable;

    template<std::size_t I, typename ...Args>
    constexpr decltype(auto) apply(std::size_t i, Args&&... args )
    noexcept(noexcept(DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS()))
    {
      // TODO: error handling anti-pattern
      using Result = std::result_of_t<F( \
        std::integral_constant<std::size_t, I>, Args...)>; \
      if constexpr (I < N) {
        switch(i) {
          case I:
            return DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS();
          default:
            return apply<I + 1>(i, std::forward<Args>(args)...);
        }
      }  else if constexpr (!std::is_same<Result, void>{}) { \
        return Result{}; \
      }
    }

    template<typename ...Args>
    constexpr decltype(auto) operator()(std::size_t i, Args&&... args)
    DISPATCH_NOEXCEPT_FUNCTION_BODY(apply<0>(i, std::forward<Args>(args)...))

  };

  template<std::size_t N, typename F>
  constexpr decltype(auto) make_sequential_table(F&& f)
  DISPATCH_NOEXCEPT_FUNCTION_BODY(SequentialTable<F, N>{std::forward<F>(f)});

#undef DISPATCH_RECURSIVE_SWITCH_TABLE_RETURNS
#undef DISPATCH_NOEXCEPT_FUNCTION_BODY

}  // namespace dispatch
