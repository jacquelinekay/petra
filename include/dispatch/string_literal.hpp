#pragma once

#include <array>
#include <string>

#include <cstring>


#include "utilities/fold.hpp"
#include "utilities/sequence.hpp"
#include "utilities/tuple.hpp"

namespace dispatch {

template<typename T, T ...Pack>
struct string_literal {
  static constexpr auto size() { return sizeof...(Pack); };
  static constexpr const char* data() { return value; };
  static constexpr auto char_at(unsigned i) { return value[i]; };
private:
  static constexpr char value[sizeof...(Pack) + 1] = {Pack..., '\0'};
};

template<typename Test, auto ...T>
struct is_string_literal;

template<typename T, T... Args>
struct is_string_literal<string_literal<T, Args...>>: std::true_type {};

template<typename Test>
struct is_string_literal<Test> : std::false_type {};

template<typename Str>
struct compare_helper {
  template<size_t... I>
  static constexpr bool apply(const char* v, std::index_sequence<I...>) {
    return ((Str::char_at(I) == v[I]) && ...);
  }
  template<typename StrB, size_t... I>
  static constexpr bool apply(std::index_sequence<I...>) {
    return ((Str::char_at(I) == StrB::char_at(I)) && ...);
  }
};

template<typename Str>
static constexpr bool empty(const Str&) {
  return Str::size() == 0;
}

template<typename StrA, typename StrB>
static constexpr bool equal(const StrA&, const StrB&) {
  if (StrA::size() != StrB::size()) {
    return false;
  } else {
    return compare_helper<StrA>::template apply<StrB>(std::make_index_sequence<StrA::size()>{});
  }
}

template<typename Str>
static constexpr bool equal(const Str&, const char* b) {
  if (utilities::length(b) != Str::size()) {
    return false;
  } else {
    return compare_helper<Str>::apply(
      b, std::make_index_sequence<Str::size()>{});
  }
}

template<typename Str,
  typename std::enable_if_t<is_string_literal<Str>{}>* = nullptr>
constexpr bool operator==(const Str& a, const char* b) {
  return equal(a, b);
}

template<typename StrA, typename StrB,
  typename std::enable_if_t<
    is_string_literal<StrA>{} &&
    is_string_literal<StrB>{}
  >* = nullptr>
constexpr bool operator==(const StrA& a, const StrB& b) {
  return equal(a, b);
}

// #ifdef DISPATCH_USE_UDL
namespace literals {
  template<typename T, T ...Pack>
  constexpr auto operator"" _s() {
    return string_literal<T, Pack...>{};
  }
}  // namespace literals
// #endif
}  // namespace dispatch
