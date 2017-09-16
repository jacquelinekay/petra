// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <limits>
#include <tuple>

#include "petra/linear_hash.hpp"
#include "petra/string_literal.hpp"
#include "petra/switch_table.hpp"
#include "petra/utilities.hpp"
#include "petra/utilities/fold.hpp"
#include "petra/utilities/sequence.hpp"
#include "petra/utilities/tuple.hpp"

namespace petra {
  // Customization point for user-provided hashable types

  static constexpr inline std::size_t chd_offset = 0x01000193;

  namespace adl {

    struct chd_tag {};

    // Precondition: D must be less than numeric_limits / chd_offset.
    static constexpr std::size_t chd(std::size_t d, std::size_t value,
                                     std::size_t size, chd_tag&&) noexcept {
      if (d == 0) {
        d = chd_offset;
      }
      return (((d * chd_offset) ^ value) & 0xffffffff) % size;
    }

    // Precondition: D must be less than numeric_limits / chd_offset.
    static constexpr std::size_t chd(std::size_t d, const char* s,
                                     std::size_t size, chd_tag&&) noexcept {
      if (*s == 0) {
        return d % size;
      }
      if (d == 0) {
        d = chd_offset;
      }
      d = ((d * chd_offset) ^ (static_cast<std::size_t>(*s))) & 0xffffffff;
      return chd(d, s + 1, size, chd_tag{});
    }

    template<typename T, T... Pack>
    static constexpr std::size_t chd(std::size_t d,
                                     const string_literal<T, Pack...>&,
                                     std::size_t size, chd_tag&&) noexcept {
      return chd(d, string_literal<T, Pack...>::value, size, chd_tag{});
    }
  }  // namespace adl

  namespace detail {

    enum struct hash_status : char { Empty, Unique, Collision };

    template<hash_status Status>
    using hash_status_constant = std::integral_constant<hash_status, Status>;

    static constexpr inline std::size_t overflow_boundary =
        std::numeric_limits<std::size_t>::max() / chd_offset;

    template<typename... Inputs>
    static constexpr auto make_unique_seq() noexcept {
      return remove_repeats(
          std::index_sequence<chd(0, Inputs{}, sizeof...(Inputs),
                                  adl::chd_tag{})...>{});
    }

    template<typename... Inputs>
    static constexpr auto initialize_dictionary() noexcept {
      using adl::chd;
      constexpr auto unique_seq = make_unique_seq<Inputs...>();

      constexpr std::size_t n_uniques = decltype(unique_seq)::size();
      static_assert(n_uniques > 0);

      constexpr auto empty_tuple = [](auto t) {
        return append(t, std::make_tuple());
      };

      constexpr auto initial_tuple = utilities::times<n_uniques - 1>(
          empty_tuple, std::make_tuple(std::make_tuple()));

      static_assert(tuple_size(initial_tuple) > 0);
      static_assert(tuple_size(initial_tuple) == n_uniques);

      constexpr auto accumulate_sets = [](const auto& dict, const auto& str) {
        constexpr auto hashed_value = chd(0, std::decay_t<decltype(str)>{},
                                          sizeof...(Inputs), adl::chd_tag{});
        constexpr std::size_t index =
            map_to_index<hashed_value>(make_unique_seq<Inputs...>());
        return insert_at<index>(dict, append(std::get<index>(dict), str));
      };
      constexpr auto result =
          fold_left(accumulate_sets, initial_tuple, Inputs{}...);

      return std::make_pair(unique_seq, result);
    }

    template<std::size_t Total, std::size_t D, typename Values>
    static constexpr auto get_unique_hash_value(Values&&) {
      return std::make_pair(
          std::make_pair(hash_status_constant<hash_status::Empty>{}, Total),
          std::index_sequence<>{});
    }
    template<std::size_t Total, std::size_t D, typename SubInput,
             typename Values>
    static constexpr auto get_unique_hash_value(Values&&) {
      // Maps directly to a unique string, so we can avoid hashing.
      return std::make_pair(
          std::make_pair(hash_status_constant<hash_status::Unique>{}, Total),
          std::index_sequence<>{});
    }

    template<std::size_t Total, std::size_t D, typename... SubInputs,
             typename Values,
             typename = std::enable_if_t<sizeof...(SubInputs) >= 2>>
    static constexpr auto get_unique_hash_value(Values&& values) {
      static_assert(D < overflow_boundary,
                    "Tried to hash value which would lead to unsigned overflow.\
         Sorry, the algorithm is not robust enough to handle your input set!");
      using adl::chd;
      constexpr auto seq =
          std::index_sequence<chd(D, SubInputs{}, Total, adl::chd_tag{})...>{};

      if constexpr (unique(seq)
                    && disjoint(seq, std::decay_t<decltype(values)>{})) {
        // Make sure to return the new values
        return std::make_pair(
            std::make_pair(hash_status_constant<hash_status::Collision>{}, D),
            seq);
      } else {
        return get_unique_hash_value<Total, D + 1, SubInputs...>(values);
      }
    }

    template<template<typename...> typename IntermediateHash,
             typename... Inputs>
    static constexpr auto construct_hash() noexcept {
      constexpr std::size_t set_size = sizeof...(Inputs);
      if constexpr (set_size <= 4) {
        return LinearHash<Inputs...>{};
      } else {
        constexpr auto dict = initialize_dictionary<Inputs...>();
        constexpr auto keys = dict.first;
        constexpr auto subsets = dict.second;

        constexpr auto unique_hash_values = [](const auto& result,
                                               auto&& inputs) {
          using Sequence = std::decay_t<decltype(result.second)>;
          constexpr auto f = [](auto&&... s) {
            return get_unique_hash_value<sizeof...(Inputs), 1,
                                         std::decay_t<decltype(s)>...>(
                Sequence{});
          };
          auto next_result = std::apply(f, inputs);
          return std::make_pair(append(result.first, next_result.first),
                                concatenate(result.second, next_result.second));
        };

        // TODO: Could simplify the result type into an array of
        // pair<hash_status, int> but only if GCC allowed constexpr lambda
        // captures
        constexpr auto disambiguate =
            [unique_hash_values](auto&&... string_list) {
              // result: pair((hash status, d), values)
              return fold_left(
                  unique_hash_values,
                  std::make_pair(std::make_tuple(), std::index_sequence<>{}),
                  string_list...);
            };

        constexpr auto disambiguated_result = std::apply(disambiguate, subsets);
        constexpr auto next_keys = disambiguated_result.first;
        constexpr auto hashed_values = disambiguated_result.second;

        using NextKeys = decltype(next_keys);

        static_assert(unique(hashed_values));

        constexpr auto freelist =
            difference(std::make_index_sequence<set_size>{}, hashed_values);

        static_assert(hashed_values.size() + freelist.size() == set_size);

        constexpr auto pop_from_freelist = [](auto&& result_freelist,
                                              auto&& i) {
          constexpr size_t index = std::decay_t<decltype(i)>::value;
          using Elem = std::tuple_element_t<index, NextKeys>;
          using Status = typename Elem::first_type;
          auto result = result_freelist.first;
          auto cur_freelist = result_freelist.second;

          if constexpr (Status::value == hash_status::Unique) {
            auto freelist_pair = pop_front(cur_freelist);
            // Swap out the typevalue with the value
            return std::make_pair(
                insert_at<index>(
                    result, std::make_pair(Status::value, freelist_pair.first)),
                freelist_pair.second);
          } else {
            return std::make_pair(
                insert_at<index>(
                    result, std::make_pair(Status::value,
                                           std::get<index>(result).second)),
                cur_freelist);
          }
        };

        constexpr auto assign_result =
            fold_left(pop_from_freelist, std::make_pair(next_keys, freelist),
                      std::make_index_sequence<tuple_size(next_keys)>{});
        constexpr auto unique_values = assign_result.first;

        // initial values maps to unique values
        using KeySequence = std::decay_t<decltype(keys)>;
        constexpr auto table_callback = [unique_values](auto&& index) noexcept {
          if constexpr (utilities::is_error_type<decltype(index)>()) {
            return std::make_pair(hash_status::Empty, 0ul);
          } else {
            constexpr auto I =
                map_to_index<std::decay_t<decltype(index)>::value>(
                    KeySequence{});

            // constexpr std::size_t Size = tuple_size(unique_values);
            constexpr std::size_t Size =
                std::tuple_size<std::decay_t<decltype(unique_values)>>::value;
            if constexpr (I >= Size) {
              return std::make_pair(hash_status::Empty, 0ul);
            } else {
              return std::get<I>(unique_values);
            }
          }
        };

        return IntermediateHash<decltype(table_callback),
                                std::decay_t<decltype(keys)>>(
            std::move(table_callback));
      }
    }

  }  // namespace detail
}  // namespace petra
