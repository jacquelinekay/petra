#pragma once

#include <utility>
#include <tuple>

#include "petra/utilities/fold.hpp"

namespace petra {
namespace utilities {

  constexpr unsigned length(const char* str) {
    if (str == nullptr) {
      return 0;
    }
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

  // from http://stackoverflow.com/questions/16337610/how-to-know-if-a-type-is-a-specialization-of-stdvector
  template<typename Test, template<typename...> class Ref>
  struct is_specialization : std::false_type {};

  template<template<typename...> class Ref, typename... Args>
  struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

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
}  // namespace petra
