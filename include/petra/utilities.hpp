// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <tuple>
#include <utility>

#include "petra/utilities/fold.hpp"

namespace petra {

  struct InvalidInputError {};

  namespace utilities {

    template<typename T>
    struct type_tag {
      using type = T;
    };

    constexpr unsigned length(const char* str) {
      if (str == nullptr) {
        return 0;
      }
      return *str ? 1 + length(str + 1) : 0;
    }

    // Hm
    template<size_t Max, typename Cur, typename... Strings>
    constexpr auto
    max_string_length_recursive(const std::tuple<Cur, Strings...>&) {
      constexpr unsigned Size = std::decay_t<Cur>::size();
      if constexpr (Size > Max) {
        if constexpr (sizeof...(Strings) > 0) {
          return max_string_length_recursive<Size>(
              std::make_tuple(Strings{}...));
        } else {
          return Size;
        }
      } else {
        if constexpr (sizeof...(Strings) > 0) {
          return max_string_length_recursive<Max>(
              std::make_tuple(Strings{}...));
        } else {
          return Max;
        }
      }
    }

    // XXX
    template<typename... Strings>
    constexpr auto max_string_length(const Strings&... strs) {
      return max_string_length_recursive<0>(std::make_tuple(strs...));
    }

    // from
    // http://stackoverflow.com/questions/16337610/how-to-know-if-a-type-is-a-specialization-of-stdvector
    template<typename Test, template<typename...> class Ref>
    struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    template<typename T>
    struct is_reference_wrapper : is_specialization<T, std::reference_wrapper> {
    };

    template<std::size_t... Sequence>
    constexpr auto as_tuple(std::index_sequence<Sequence...>) {
      return std::make_tuple(Sequence...);
    }

    template<typename F, typename T, std::size_t... I>
    constexpr auto times_helper(F&& f, T&& initial, std::index_sequence<I...>) {
      return fold_left([f](auto t, auto) { return f(t); }, initial, I...);
    }

    template<std::size_t I, typename F, typename T>
    constexpr auto times(F&& f, T&& initial) {
      if constexpr (I == 0) {
        return initial;
      } else {
        return times_helper(f, initial, std::make_index_sequence<I>{});
      }
    }

    template<typename T>
    constexpr bool is_error_type() {
      return std::is_same<std::decay_t<T>, InvalidInputError>{};
    }

    template<typename T>
    static T constexpr abs(T i) {
      if constexpr (std::is_signed<T>{}) {
        if (i < 0) {
          return i * -1;
        }
      }
      return i;
    }

    template<typename T, auto Size>
    const T& at(const std::array<T, Size>& array, std::size_t pos) noexcept {
      return array[pos];
    }

  }  // namespace utilities
}  // namespace petra
