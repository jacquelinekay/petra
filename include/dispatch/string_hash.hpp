#pragma once

#include <tuple>

#include "dispatch/recursive_switch_table.hpp"
#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"

namespace sl = jk::string_literal;

template<template<size_t> typename F, typename StringSet>
constexpr auto make_string_map_naive(StringSet&& string_set) {
  return make_string_map_naive_helper(string_set, std::make_index_sequence<std::tuple_size<StringSet>{}>{});
}

template<typename ...Strings>
constexpr unsigned max_string_length(Strings&&...) {
  return ([](auto max) {
    constexpr unsigned i = Strings::value().size();
    if constexpr (i > decltype(max){}) {
      return i;
    } else {
      return max;
    }
  }(std::integral_constant<unsigned, 0>{}), ...);
}

template<char ...Pack1, char ...Pack2>
constexpr bool pack_compare(std::integer_sequence<char, Pack1...>&&, std::integer_sequence<char, Pack2...>&&) {
  if constexpr (sizeof...(Pack1) != sizeof...(Pack2)) {
    return false;
  } else {
    return ((Pack1 == Pack2) && ...);
  }
}

template<size_t J, typename Tuple, size_t ...K>
constexpr bool test_byte_positions_helper2(Tuple&& t, std::index_sequence<K...>&&) {
  using DTuple = std::decay_t<Tuple>;
  return ([&t](auto&& x) {
      if constexpr (J == K) {
        return x;
      } else if constexpr (pack_compare(std::tuple_element_t<J, DTuple>{}, std::tuple_element_t<K, DTuple>{})) {
        return std::false_type{};
      } else {
        return x;
      }
    }(std::true_type{}), ...);
}

template<typename Tuple, size_t ...K>
constexpr bool test_byte_positions_helper(Tuple&& t, std::index_sequence<K...>&&) {
  using DTuple = std::decay_t<Tuple>;
  constexpr auto n_strings = std::tuple_size<DTuple>{};
  return (test_byte_positions_helper2<K>(t,
    std::make_index_sequence<n_strings>{}) && ...);
}

template<typename Positions, typename ...Strings>
constexpr bool test_byte_positions(Positions&&, Strings&&...);

template<size_t ...I, typename ...Strings>
constexpr bool test_byte_positions(std::index_sequence<I...>&& i, Strings&&... strings) {
  constexpr auto make_string = [](auto&& string, std::index_sequence<I...>&&) {
    return std::integer_sequence<char, std::decay_t<decltype(string)>::value().data()[I]...>{};
  };

  // TODO: Shorten I... if the string is smaller
  constexpr auto candidate_strings = std::make_tuple(make_string(Strings{}, decltype(i){})...);

  return test_byte_positions_helper(candidate_strings,
    std::make_index_sequence<std::tuple_size<decltype(candidate_strings)>{}>{});
}

// Do this until a pass over 0 results in no changed entries.
template<typename ...Strings, size_t ...I>
constexpr auto find_byte_positions_helper(Strings&&...) {
  // Try removing one entry from I
  // If test_byte_positions succeeds, 
}

/* Step 1 (Finding good byte positions):
 * Find a set Pos, as small as possible, such that all tuples
 * (keyword[i] : i in Pos) are different.
 */
template<typename ...Strings>
constexpr auto find_byte_positions(Strings&&...) {
  // Could start with (0, ..., maxlength)
  // then try removing an entry and testing the condition
  // terminate when we've concluded we can't remove any more
  constexpr unsigned max_length = max_string_length(Strings{}...);

  constexpr auto initial_positions = std::make_index_sequence<max_length>{};

  // TODO Argh
  // static_assert(test_byte_positions(std::make_index_sequence<max_length>{}, Strings{}...));

  // TODO optimize to get a smaller, faster hash
  return initial_positions;
}


constexpr bool test_alpha_increments() {
  // TODO
  return true;
}

/* Step 2 (Finding good alpha increments):
 * Find nonnegative integers alpha_inc[i], as many of them as possible being
 * zero, and the others being as small as possible, such that all multisets
 * {keyword[i] + alpha_inc[i] : i in Pos} are different.
 */
template<size_t ...I, typename ...Strings>
constexpr auto find_alpha_increments(const std::index_sequence<I...>& positions, Strings&&...) {
  // initialize alpha_inc to all zeros, same size as Pos
  constexpr auto alpha_inc = std::make_index_sequence<sizeof...(I)>{};
  // make the multisets {keyword[i] + alpha_inc[i] : i in Pos}
  // adjust until smallest size found
  // TODO optimize!
  return alpha_inc;
}

constexpr bool test_associative_values() {
  // TODO run the hash on all strings and check that the results are not equal
  return true;
}

/* Step 3 (Finding good asso_values):
 * Find asso_values[c] such that all hash (keyword) are different.
 */
template<typename Positions, typename AlphaIncs, typename ...Strings>
constexpr auto find_associative_values(Positions&&, AlphaIncs&&, Strings&&...) {
  // how to build and **represent** asso_values?
  // key set: all possible indices that result from keyword[i] + alpha_inc[i] : i in Pos
  // run through the hash function and adjust until correct
  // TODO
  // I think mapping keys directly to the values might just work?
  // will just mapping keys directly to values will work but be pessimistic?
  // so right now associative_values is literally nothing
  return 0;
}

template<typename AssocValues>
constexpr auto hash_get(AssocValues&&, unsigned i) {
  return i;
}

template<typename AssocValues, auto I>
constexpr auto chash_get(const AssocValues&) {
  return I;
}

/*
 * from GNU gperf (https://github.com/yakaz/gperf/blob/output-javascript/src/search.cc):
 *  hash (keyword) = sum (asso_values[keyword[i] + alpha_inc[i]] : i in Pos)
 *                       + len (keyword)
 */
template<typename Positions, typename AlphaIncs, typename AssocValues>
struct compute_hash {
  // TODO: Add int hash to matching to sequential integers
  template<size_t ...I>
  static auto runtime_compute_helper(const char* keyword, const std::index_sequence<I...>&) {
    return (hash_get(associative_values, keyword[I] + access_sequence<I>(AlphaIncs{})) + ...);
  }

  auto operator()(const char* keyword) const {
    return runtime_compute_helper(keyword, positions) + strlen(keyword);
  }

  template<typename StringLiteral, size_t ...I>
  static constexpr auto compute_helper(StringLiteral&&, const std::index_sequence<I...>&) {
    /*
    return (chash_get<StringLiteral::value().data()[I]
      + access_sequence<I>(alpha_increments)>(associative_values) + ...);
    */
    // TODO: hash access assoc. values
    return ((StringLiteral::value().data()[I] + access_sequence<I>(AlphaIncs{})) + ...);
  }

  // Constexpr version
  template<typename StringLiteral>
  static constexpr auto hash(StringLiteral&&) {
    return compute_helper(StringLiteral{}, positions) + StringLiteral::value().size();
  }

  static constexpr Positions positions;
  static constexpr AlphaIncs alpha_increments;
  static constexpr AssocValues associative_values;
};

template<typename ...Strings>
constexpr auto prepare_string_hash(Strings&&...) {
  constexpr auto byte_positions = find_byte_positions(Strings{}...);
  constexpr auto alpha_increments = find_alpha_increments(byte_positions, Strings{}...);
  constexpr auto associative_values = find_associative_values(byte_positions, alpha_increments, Strings{}...);

  return compute_hash<decltype(byte_positions), decltype(alpha_increments), decltype(associative_values)>{};
}

template<template<size_t> typename F, typename ...Strings>
struct string_dispatch {
  static constexpr auto string_hash = prepare_string_hash(Strings{}...);
  static constexpr recursive_switch_table<F, string_hash.hash(Strings{})...> table;

  auto operator()(const char* value) const {
    return table(string_hash(value));
  }
};

template<template<size_t> typename F, typename ...Strings>
constexpr auto make_string_dispatch(Strings&&...) {
  return string_dispatch<F, Strings...>{};
}

