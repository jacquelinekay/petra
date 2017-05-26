#pragma once

#include <tuple>
#include <utility>
#include "dispatch/string_literal.hpp"

#include <iostream>

namespace sl = dispatch::string_literal;

template<typename F, typename StringSet>
struct naive_string_hash {
  F callable;

  template<std::size_t ...I>
  void helper(std::index_sequence<I...>, const char* input) {
    return ([this](const char* input) {
      if (sl::equal(std::tuple_element_t<I, StringSet>{}, input)) {
        callable(std::integral_constant<std::size_t, I>{});
      }
    }(input), ...);
  }

  void operator()(const char* input) {
    helper(std::make_index_sequence<std::tuple_size<StringSet>{}>{}, input);
  }
};

template<typename F, typename... Strings>
constexpr auto make_naive_string_hash(F&& callable, Strings&&... strings) {
  return naive_string_hash<F, std::tuple<Strings...>>{callable};
}
