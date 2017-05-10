#pragma once

#include <utility>

#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"

namespace sl = jk::string_literal;

// Disadvantages:
// Runtime complexity is O(n) where n is the length of the longest string key
// Does not work with reverse words (for example "abcd" and "dcba" will collide)

template<typename... Strings>
struct simple_string_hash {
  static constexpr auto positions = std::make_index_sequence<get_max_string_length(Strings{}...)>{};
  static constexpr auto increments = std::make_index_sequence<decltype(positions)::size()>{};

  template<size_t ...I>
  static auto runtime_compute_helper(const char* keyword, const std::index_sequence<I...>&) {
    return ((keyword[I] + access_sequence<I>(increments)) + ...);
  }

  auto operator()(const char* keyword) const {
    return runtime_compute_helper(keyword, positions) + strlen(keyword);
  }

  template<typename StringLiteral, size_t ...I>
  static constexpr auto compute_helper(StringLiteral&&, const std::index_sequence<I...>&) {
    return ((StringLiteral::value().data()[I] + access_sequence<I>(increments)) + ...);
  }

  template<typename StringLiteral>
  static constexpr auto hash(StringLiteral&&) {
    return compute_helper(StringLiteral{}, positions) + StringLiteral::value().size();
  }
};

template<typename... Strings>
static constexpr auto make_simple_string_hash(Strings&&...) {
  return simple_string_hash<Strings...>{};
}
