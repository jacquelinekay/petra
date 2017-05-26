#pragma once

#include <tuple>
#include <variant>

#include "dispatch/dictionary.hpp"
#include "dispatch/recursive_switch_table.hpp"
#include "dispatch/string_dispatch.hpp"
#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"

namespace dispatch {

  enum struct hash_status {
    Empty,
    Unique,
    Collision
  };

  template<auto Val>
  struct Constant {
    static constexpr auto value = Val;
  };

  template<typename ...Strings>
  struct MinimalHash {
    // TODO: use enable_if when the string set has 4 or fewer words
    static constexpr std::size_t set_size = sizeof...(Strings);
    static_assert(set_size > 4);

    // TODO: Tune this seed at compile time?
    static constexpr inline std::size_t distinct_offset = 0x01000193;

    static constexpr std::size_t distinct_hash_helper(std::size_t d, const char* s) {
      if (*s == 0) {
        return d;
      }
      if (d == 0) {
        d = distinct_offset;
      }
      d = ((d * distinct_offset) ^ (static_cast<std::size_t>(*s))) & 0xffffffff;
      return distinct_hash_helper(d, s + 1);
    }

    static constexpr std::size_t distinct_hash(std::size_t d, const char* s, std::size_t size = set_size) {
      if (!s) {
        // TODO
        return d;
      }
      return distinct_hash_helper(d, s) % size;
    }

    static constexpr auto initialize_dictionary() {
      constexpr auto unique_sequence = make_unique_sequence(std::index_sequence<>{},
              std::index_sequence<distinct_hash(0, Strings::value().data())...>{});

      constexpr auto empty_tuple = [](auto t) {
        return append(t, std::make_tuple());
      };

      constexpr std::size_t n_uniques = decltype(unique_sequence)::size();
      static_assert(n_uniques > 0);
      constexpr auto initial_tuple = times<n_uniques - 1>(
          empty_tuple, std::make_tuple(std::make_tuple()));

      static_assert(tuple_size(initial_tuple) > 0);
      static_assert(tuple_size(initial_tuple) == n_uniques);

      constexpr auto accumulate_strings = [unique_sequence](auto&& t, auto&& str) {
        constexpr auto value = std::decay_t<decltype(str)>::value().data();
        constexpr auto hashed_value = distinct_hash(0, value);
        constexpr std::size_t index = map_to_index<hashed_value>(unique_sequence);
        return insert_at<index>(t, append(std::get<index>(t), str));
      };
      return std::make_pair(unique_sequence, fold_left(accumulate_strings, initial_tuple, Strings{}...));
    }

    // TODO: This is wrong. Need to compare the resulting hash values across **all** subsets
    template<std::size_t D, typename ...SubsetString, typename Values>
    static constexpr auto get_unique_hash_value(Values&& values) {
      constexpr std::size_t subset_size = sizeof...(SubsetString);
      if constexpr (subset_size == 0) {
        return std::make_pair(std::make_pair(hash_status::Empty, 0), std::index_sequence<>{});
      } else if constexpr (subset_size == 1) {
        // Maps directly to a unique string, so we can avoid hashing.
        return std::make_pair(std::make_pair(hash_status::Unique, 0), std::index_sequence<>{});
      } else {
        constexpr auto unique_sequence = make_unique_sequence(std::index_sequence<>{},
            std::index_sequence<distinct_hash(D, SubsetString::value().data())...>{});
        // check that this the resulting values are disjoint from the current values

        if constexpr (disjoint(unique_sequence, std::decay_t<decltype(values)>{})) {
          // Make sure to return the new values
          return std::make_pair(
              std::make_pair(hash_status::Collision, D),
              unique_sequence);
        } else {
          return get_unique_hash_value<D + 1, SubsetString...>(values);
        }
      }
    }

    static constexpr auto construct_hash() {
      constexpr auto dict = initialize_dictionary();
      constexpr auto initial_keys = dict.first;
      constexpr auto initial_values = dict.second;

      // for each set of strings, find a value which hashes them to unique values

      constexpr auto second_level = [](auto&&... string_list) {
        // result: pair((hash status, d), values)
        constexpr auto unique_hash_values = [](auto&& result, auto strings) {
          auto f = [result](auto&&... s) {
            auto next = get_unique_hash_value<1, std::decay_t<decltype(s)>...>(result.second);
            return std::pair(append(result.first, next.first), concatenate(result.second, next.second));
          };
          return std::apply(f, strings);
        };

        return fold_left(
            unique_hash_values,
            std::make_pair(std::make_tuple(), std::index_sequence<>{}),
            string_list...);
      };
      constexpr auto second_level_results = std::apply(second_level, initial_values).first;

      constexpr auto filter_collisions = [second_level_results](auto&& seq, auto&& i) {
        constexpr size_t index = std::decay_t<decltype(i)>::value;
        constexpr auto status = std::get<index>(second_level_results).first;
        if constexpr (status == hash_status::Collision) {
          return append<index>(seq);
        } else {
          return seq;
        }
      };
      constexpr auto filtered_seq = fold_left(filter_collisions, std::index_sequence<>{},
          std::make_index_sequence<tuple_size(second_level_results)>{});

      constexpr auto do_hash = [second_level_results, initial_values, filtered_seq](auto&& result, auto&& i) {
        constexpr std::size_t index = std::decay_t<decltype(i)>::value;
        if constexpr (in_sequence(index, filtered_seq)) {
          constexpr auto f = [second_level_results](auto&&... strs) {
            return std::index_sequence<
              distinct_hash(std::get<index>(second_level_results).second,
                  std::decay_t<decltype(strs)>::value().data())...
            >{};
          };
          return concatenate(result, std::apply(f, std::get<index>(initial_values)));
        } else {
          return result;
        }
      };
      constexpr auto hashed_values = fold_left(do_hash, std::index_sequence<>{},
          std::make_index_sequence<initial_keys.size()>{});

      constexpr auto freelist = difference(std::make_index_sequence<set_size>{}, hashed_values);

      constexpr auto assign_from_freelist = [second_level_results](auto&& result_freelist, auto&& i) {
        constexpr size_t index = std::decay_t<decltype(i)>::value;
        constexpr auto status = std::get<index>(second_level_results).first;

        if constexpr (status == hash_status::Unique) {
          auto result = result_freelist.first;
          auto cur_freelist = result_freelist.second;
          auto freelist_pair = pop_front(cur_freelist);
          return std::make_pair(
              insert_at<index>(result, std::make_pair(status, freelist_pair.first)), freelist_pair.second);
        } else {
          return result_freelist;
        }
      };

      constexpr auto assign_result = fold_left(
          assign_from_freelist,
          std::make_pair(second_level_results, freelist),
          std::make_index_sequence<tuple_size(second_level_results)>{});
      constexpr auto unique_values = assign_result.first;

      // TODO: Prune Empty values
      // initial values maps to unique values
      return make_recursive_switch_table(
        [initial_keys, unique_values](auto&& index) {
        // TODO: Encapsulate switching here
          constexpr auto I = map_to_index<std::decay_t<decltype(index)>{}>(initial_keys);

          if constexpr (I >= tuple_size(unique_values)) {
            return std::make_pair(hash_status::Empty, 0);
          } else {
            return std::get<I>(unique_values);
          }
        },
        initial_keys);
    }

    static constexpr auto intermediate_hash = construct_hash();

    static constexpr auto hash(const char* str) {
      // TODO: len(G) != set_size, so this differs from the Python impl
      std::size_t key = distinct_hash(0, str);
      std::cout << "hashed to key: " << key << "\n";
      // TODO 
      const auto& [status, d] = intermediate_hash(key);
      switch(status) {
        case hash_status::Unique:
          return d;
        case hash_status::Collision:
          return distinct_hash(d, str);  // TODO: len(V)?
        case hash_status::Empty:
        default:
          return set_size;
      }
    }

    constexpr MinimalHash(Strings&&... strings) {
    }
  };

  

  template<typename... Args>
  static constexpr auto make_minimal_hash(Args&&... args) {
    return MinimalHash(string_literal::string_constant(args, utilities::length(args))...);
  }

}  // namespace dispatch
