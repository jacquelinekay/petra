#pragma once

#include "dispatch/simple_string_hash.hpp"
#include "dispatch/string_dispatch.hpp"

#include <iostream>
#include <iostream>
#include <tuple>

namespace dispatch {
namespace test_utils {

// Shared utilities for testing string hash
// TODO: Clean up the globals into a shared context
static constexpr size_t set_size = 10;

constexpr auto example_constants() {
  return std::make_tuple(
    STRING_LITERAL("asdf"),
    STRING_LITERAL("qwerty"),
    STRING_LITERAL("quux"),
    STRING_LITERAL("int"),
    STRING_LITERAL("arguments"),
    STRING_LITERAL("foobar"),
    STRING_LITERAL("abcd"),
    STRING_LITERAL("badc"),
    STRING_LITERAL("foo"),
    STRING_LITERAL("oof")
  );
}

constexpr auto example_test_strings() {
  return std::array<const char*, set_size>{{
    "asdf",
    "qwerty",
    "quux",
    "int",
    "arguments",
    "foobar",
    "abcd",
    "badc",
    "foo",
    "oof",
  }};
}

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) {
    if (N < set_size) {
      std::cout << N << "\n";
      ++results[N];
      assert(results[N] == 1);
    }
  }
  std::array<int, set_size> results = {{0}};
};

template<typename T, size_t... I>
constexpr auto dispatch_table_from_tuple_helper(
    const T&, std::index_sequence<I...>&&) {
  return make_string_dispatch<simple_string_hash, test>(
    test{},
    std::tuple_element_t<I, T>{}...
  );
}

template<typename T>
constexpr auto dispatch_table_from_tuple(const T& strings) {
  return dispatch_table_from_tuple_helper(
      strings, std::make_index_sequence<std::tuple_size<T>{}>{});
}

template<typename T, size_t... I>
constexpr auto max_string_length_helper(
    const T& strings, std::index_sequence<I...>) {
  return utilities::max_string_length(std::get<I>(strings)...);
}

template<typename T>
constexpr auto max_string_length_tuple(const T& strings) {
  return max_string_length_helper(
      strings, std::make_index_sequence<std::tuple_size<T>{}>{});
}

template<typename Table, typename StringTuple, typename Literals, size_t... I>
bool runtime_compiletime_match(
    const Table& table,
    const StringTuple& string_tuple,
    const Literals& test_strings) {
  return ((table.string_hash.hash(std::get<I>(string_tuple))
      == table.string_hash(test_strings[I])) && ...);
}


template<size_t I, typename Table, typename StringTuple, size_t... J>
constexpr bool compare_single_hash(
    const Table& table,
    const StringTuple& strings,
    std::index_sequence<J...>&&) {
  return ([&table, &strings](auto result) {
    if (I == J) {
      return result;
    } else {
      return table.string_hash.hash(std::get<I>(strings))
        != table.string_hash.hash(std::get<J>(strings));
    }
  }(true), ...);
}

// static_assert that the hash values are unique
template<typename Table, typename StringTuple, size_t... I>
constexpr bool unique_hashes(
    const Table& table,
    const StringTuple& strings,
    std::index_sequence<I...>&&) {
  return (compare_single_hash<I>(
            table, strings, std::make_index_sequence<sizeof...(I)>{})
        && ...);
}


}  // namespace test_utils
}  // namespace dispatch
