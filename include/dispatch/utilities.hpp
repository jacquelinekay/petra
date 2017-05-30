#pragma once

#include <experimental/type_traits>
#include <utility>
#include <tuple>

#include "dispatch/utilities/fold.hpp"

namespace dispatch {
namespace utilities {

  constexpr unsigned length(const char* str) {
    return *str ? 1 + length(str + 1) : 0;
  }

  // Hm
  template<size_t Max, typename Cur, typename ...Strings>
  constexpr auto max_string_length_recursive(const std::tuple<Cur, Strings...>&) {
    constexpr unsigned Size = std::decay_t<Cur>::size();
    if constexpr (Size > Max) {
      if constexpr (sizeof...(Strings) > 0) {
        return max_string_length_recursive<Size>(std::make_tuple(Strings{}...));
      } else {
        return Size;
      }
    } else {
      if constexpr (sizeof...(Strings) > 0) {
        return max_string_length_recursive<Max>(std::make_tuple(Strings{}...));
      } else {
        return Max;
      }
    }
  }

  // XXX
  template<typename ...Strings>
  constexpr auto max_string_length(const Strings&... strs) {
    return max_string_length_recursive<0>(std::make_tuple(strs...));
  }

  template<template<typename ...> typename Op, typename... Args>
  using is_detected = std::experimental::is_detected<Op, Args...>;

  template<typename T, typename S>
  using comparable_exp = decltype(
      std::declval<std::decay_t<T>>() == std::declval<std::decay_t<S>>());

  template<typename T, typename S>
  using comparable = is_detected<comparable_exp, T, S>;

  template<std::size_t... Sequence>
  constexpr auto as_tuple(std::index_sequence<Sequence...>) {
    return std::make_tuple(Sequence...);
  }

  template<typename F, typename T, std::size_t ...I>
  constexpr auto times_helper(F&& f, T&& initial, std::index_sequence<I...>) {
    return fold_left(
      [f](auto t, auto) {
        return f(t);
      },
      initial, I...
    );
  }

  template<std::size_t I, typename F, typename T>
  constexpr auto times(F&& f, T&& initial) {
    if constexpr (I == 0) {
      return initial;
    } else {
      return times_helper(f, initial, std::make_index_sequence<I>{});
    }
  }

  // XXX
  /*
  template<typename Product>
  struct product_to_sum {
    using type = decltype(std::apply(unwrap, std::declval<Product>()));
  private:
    static constexpr auto unwrap = [](auto&&... args) {
      return std::variant<std::decay_t<decltype(args)>...>{};
    }
  };
  */

}  // namespace utilities
}  // namespace dispatch
