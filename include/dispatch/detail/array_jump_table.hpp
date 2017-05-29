
#pragma once

#include <array>
#include <utility>

namespace dispatch {
namespace detail {

#define DISPATCH_NOEXCEPT_LAMBDA_RETURNS(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { \
    return __VA_ARGS__; \
  } \

  template<template<std::size_t> typename F, std::size_t N, typename ...Args>
  struct ArrayTagTable {

    using return_type = std::result_of_t<F<0>(Args&&...)>;
    using table_type = std::array<return_type(*)(Args&&...), N>;

    // Precondition: i **must** be less than n_callbacks.
    return_type operator()(std::size_t i, Args&&... args) {
      return table[i](std::forward<Args>(args)...);
    }

  private:

    template<std::size_t I>
    static constexpr decltype(auto) table_entry() {
      return +[](Args&&... args)
        DISPATCH_NOEXCEPT_LAMBDA_RETURNS(
          F<I>{}(std::forward<Args>(args)...)
        );
    }

    template<std::size_t ...I>
    static constexpr auto init_table(std::index_sequence<I...>) {
      // static_assert(std::is_same<return_type, std::result_of_t<F<I>()>>{} && ...);
      return table_type{{ table_entry<I>()... }};
    }

    static constexpr table_type table = init_table(std::make_index_sequence<N>{});
  };


#undef DISPATCH_NOEXCEPT_FUNCTION_BODY

}  // namespace detail
}  // namespace dispatch
