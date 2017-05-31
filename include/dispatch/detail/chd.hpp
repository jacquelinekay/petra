#pragma once

#include <tuple>

#include "dispatch/linear_hash.hpp"
#include "dispatch/switch_table.hpp"
#include "dispatch/string_dispatch.hpp"
#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"
#include "dispatch/utilities/fold.hpp"
#include "dispatch/utilities/sequence.hpp"
#include "dispatch/utilities/tuple.hpp"

namespace dispatch {
namespace detail {

  enum struct hash_status : char {
    Empty,
    Unique,
    Collision
  };

  static constexpr inline std::size_t distinct_offset = 0x01000193;
  static constexpr inline std::size_t overflow_boundary =
      std::numeric_limits<std::size_t>::max() / 0x01000193;

  static constexpr std::size_t distinct_hash(
      std::size_t d, std::size_t value, std::size_t size) {
    if (d == 0) {
      d = distinct_offset;
    }
    return (((d * distinct_offset) ^ value) & 0xffffffff) % size;
  }

  static constexpr std::size_t distinct_hash(
      std::size_t d, const char* s, std::size_t size) {
    if (*s == 0) {
      return d % size;
    }
    if (d == 0) {
      d = distinct_offset;
    }
    d = ((d * distinct_offset) ^ (static_cast<std::size_t>(*s))) & 0xffffffff;
    return distinct_hash(d, s + 1, size);
  }

  template<typename String>
  static constexpr std::size_t distinct_hash(
      std::size_t d, String&&, std::size_t size) {
    return distinct_hash(d, std::decay_t<String>::data(), size);
  }

  template<typename ...Inputs>
  static constexpr auto initialize_dictionary() noexcept {
    constexpr std::size_t set_size = sizeof...(Inputs);
    constexpr auto unique_seq = remove_repeats(
        std::index_sequence<distinct_hash(0, Inputs{}, set_size)...>{});

    constexpr std::size_t n_uniques = decltype(unique_seq)::size();
    static_assert(n_uniques > 0);

    constexpr auto empty_tuple = [](auto t) {
      return append(t, std::make_tuple());
    };

    constexpr auto initial_tuple = utilities::times<n_uniques - 1>(
        empty_tuple, std::make_tuple(std::make_tuple()));

    static_assert(tuple_size(initial_tuple) > 0);
    static_assert(tuple_size(initial_tuple) == n_uniques);

    constexpr auto accumulate_sets = [unique_seq](auto&& dict, auto&& str) {
      constexpr auto value = std::decay_t<decltype(str)>::data();
      constexpr auto hashed_value = distinct_hash(0, value, set_size);
      constexpr std::size_t index = map_to_index<hashed_value>(unique_seq);
      return insert_at<index>(dict, append(std::get<index>(dict), str));
    };
    return std::make_pair(unique_seq,
        fold_left(accumulate_sets, initial_tuple, Inputs{}...));
  }

  template<std::size_t Total, std::size_t D,
           typename ...SubInputs, typename Values>
  static constexpr auto get_unique_hash_value(Values&& values) noexcept {
    static_assert(D < overflow_boundary,
        "Tried to hash value which would lead to unsigned overflow.\
         Sorry, the algorithm is not robust enough to handle your input set!");
    constexpr std::size_t subset_size = sizeof...(SubInputs);

    if constexpr (subset_size == 0) {
      return std::make_pair(
          std::make_pair(hash_status::Empty, Total),
          std::index_sequence<>{});
    } else if constexpr (subset_size == 1) {
      // Maps directly to a unique string, so we can avoid hashing.
      return std::make_pair(
          std::make_pair(hash_status::Unique, Total),
          std::index_sequence<>{});
    } else {
      constexpr auto seq = std::index_sequence<
            distinct_hash(D, SubInputs{}, Total)...>{};

      if constexpr (unique(seq) && disjoint(seq, std::decay_t<decltype(values)>{})) {
        // Make sure to return the new values
        return std::make_pair(std::make_pair(hash_status::Collision, D), seq);
      } else {
        return get_unique_hash_value<Total, D + 1, SubInputs...>(values);
      }
    }
  }

  template<template<typename...> typename IntermediateHash, typename ...Inputs>
  static constexpr auto construct_hash() {
    if constexpr (sizeof...(Inputs) <= 4) {
      return LinearHash<Inputs...>{};
    } else {
      constexpr std::size_t set_size = sizeof...(Inputs);
      constexpr auto dict = initialize_dictionary<Inputs...>();
      constexpr auto keys = dict.first;
      constexpr auto subsets = dict.second;

      constexpr auto disambiguate = [](auto&&... string_list) {
        // result: pair((hash status, d), values)
        constexpr auto unique_hash_values = [](auto&& result, auto&& inputs) {
          auto f = [result](auto&&... s) {
            auto next_result = get_unique_hash_value<
                    set_size, 1, std::decay_t<decltype(s)>...>(
                result.second);
            return std::pair(
                append(result.first, next_result.first),
                concatenate(result.second, next_result.second));
          };
          return std::apply(f, inputs);
        };

        return fold_left(
            unique_hash_values,
            std::make_pair(std::make_tuple(), std::index_sequence<>{}),
            string_list...);
      };
      constexpr auto disambiguated_result = std::apply(disambiguate, subsets);
      constexpr auto next_keys = disambiguated_result.first;
      constexpr auto hashed_values = disambiguated_result.second;

      static_assert(unique(hashed_values));

      constexpr auto freelist = difference(
          std::make_index_sequence<set_size>{},
          hashed_values);

      static_assert(hashed_values.size() + freelist.size() == set_size);

      constexpr auto pop_from_freelist =
        [next_keys](auto&& result_freelist, auto&& i) {
          constexpr size_t index = std::decay_t<decltype(i)>::value;
          constexpr auto status = std::get<index>(next_keys).first;

          if constexpr (status == hash_status::Unique) {
            auto result = result_freelist.first;
            auto cur_freelist = result_freelist.second;
            auto freelist_pair = pop_front(cur_freelist);
            return std::make_pair(
                insert_at<index>(
                    result, std::make_pair(status, freelist_pair.first)),
                freelist_pair.second);
          } else {
            return result_freelist;
          }
        };

      constexpr auto assign_result = fold_left(
          pop_from_freelist,
          std::make_pair(next_keys, freelist),
          std::make_index_sequence<tuple_size(next_keys)>{});
      constexpr auto unique_values = assign_result.first;

      // initial values maps to unique values
      constexpr auto table_callback = [keys, unique_values](auto&& index) {
          constexpr auto I = map_to_index<std::decay_t<decltype(index)>{}>(keys);

          if constexpr (I >= tuple_size(unique_values)) {
            return std::make_pair(hash_status::Empty, 0);
          } else {
            return std::get<I>(unique_values);
          }
        };
      return IntermediateHash<decltype(table_callback),
                              std::decay_t<decltype(keys)>,
                              std::decay_t<std::pair<hash_status, std::size_t>>
                              >(
                                std::move(table_callback),
                                std::make_pair(hash_status::Empty, 0));
    }
  }

}  // namespace detail
}  // namespace dispatch
