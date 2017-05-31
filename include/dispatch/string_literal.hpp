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

template<typename T, T... P>
struct compare_helper {
  template<size_t... I>
  static constexpr bool apply(const char* v, std::index_sequence<I...>) {
    return ((P == v[I]) && ...);
  }
  template<typename S, S... Q>
  static constexpr bool apply() {
    return ((P == Q) && ...);
  }
};

template<typename T, T... P>
static constexpr bool empty(const string_literal<T, P...>&) {
  return sizeof...(P) == 0;
}

template<typename T, T... P, typename S, S... Q>
static constexpr bool equal(
    const string_literal<T, P...>&, const string_literal<S, Q...>&) {
  if (sizeof...(P) != sizeof...(Q)) {
    return false;
  } else {
    return compare_helper<T, P...>::template apply<S, Q...>(
        std::make_index_sequence<sizeof...(P)>{});
  }
}

template<typename T, T... P>
static constexpr bool equal(const string_literal<T, P...>&, const char* b) {
  if (utilities::length(b) != sizeof...(P)) {
    return false;
  } else {
    return compare_helper<T, P...>::apply(b, std::make_index_sequence<sizeof...(P)>{});
  }
}

template<typename T, T... P>
constexpr bool operator==(const string_literal<T, P...>& a, const char* b) {
  return equal(a, b);
}

template<typename T, T... P, typename S, S... Q>
constexpr bool operator==(
    const string_literal<T, P...>& a, const string_literal<S, Q...>& b) {
  return equal(a, b);
}

// TODO
// #ifdef DISPATCH_USE_UDL
namespace literals {
  template<typename T, T ...Pack>
  constexpr auto operator"" _s() {
    return string_literal<T, Pack...>{};
  }
}  // namespace literals
// #endif
}  // namespace dispatch
