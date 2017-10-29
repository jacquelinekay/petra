// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <functional>
#include <type_traits>

#include "petra/chd.hpp"
#include "petra/detail/index_map.hpp"
#include "petra/expected.hpp"
#include "petra/sequential_table.hpp"
#include "petra/utilities/tuple.hpp"

namespace petra {
  /* A heterogenous map with frozen keys known at compile time.
   * Allows runtime lookup.
   * Types of values are fixed at compile time, but values are mutable.
   *
   * The interface is variant-like--considering naming this "VariantMap".
   * */

  enum MapAccessStatus { key_type_mismatch, invalid_key, success };

  template<template<typename...> typename Hash, typename Values,
           typename... Keys>
  struct Map {
    Map(Values&& v) : values(v) {}

    template<typename Value, typename Key>
    constexpr Expected<std::reference_wrapper<const Value>, MapAccessStatus>
    at(Key&& k) const {
      return set_pointer_hash(
          key_hash(k), values,
          utilities::type_tag<std::reference_wrapper<const Value>>{});
    }

    template<typename Value, typename Key>
    constexpr Expected<std::reference_wrapper<Value>, MapAccessStatus>
    at(Key&& k) {
      return set_pointer_hash(
          key_hash(k), values,
          utilities::type_tag<std::reference_wrapper<Value>>{});
    }

    // returns a read-only pointer to the location where v was inserted,
    // or nullptr if lookup failed
    template<typename Key, typename Value>
    constexpr MapAccessStatus insert(Key&& k, Value&& v) {
      return set_value_hash(key_hash(k), values, std::forward<Value>(v));
    }

    template<typename Key, typename Visitor>
    constexpr decltype(auto) visit(Key&& k, Visitor&& visitor) {
      return visitor_hash(key_hash(k), values, std::forward<Visitor>(visitor));
    }

    constexpr Expected<const char*, MapAccessStatus>
    key_at(std::size_t i) const {
      using E = Expected<const char*, MapAccessStatus>;
      return make_sequential_table<size>([](auto&& i, auto&& k) {
        using T = std::decay_t<decltype(i)>;
        if constexpr (invalid_input<T, size>()) {
          return E(MapAccessStatus::key_type_mismatch);
        } else {
          constexpr std::size_t I = T::value;
          return E(std::get<I>(k).value);
        }
      })(i, keys);
    }

    static constexpr std::size_t size = sizeof...(Keys);

  private:
    static constexpr auto key_hash = make_chd<Hash, Keys...>();

    std::tuple<Keys...> keys;
    Values values;

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(index_map_t{});

    static constexpr auto set_pointer_hash =
        make_sequential_table<size>([](auto&& index, auto& vs, auto type_tag) {
          using R = typename decltype(type_tag)::type;
          using E = Expected<R, MapAccessStatus>;
          using Vs = std::decay_t<decltype(vs)>;
          using T = std::decay_t<decltype(index)>;
          if constexpr (invalid_input<T, size>()) {
            return E(MapAccessStatus::invalid_key);
          } else {
            constexpr std::size_t Index = index_map[T::value];
            if constexpr (std::is_same<std::tuple_element_t<Index, Vs>,
                                       typename R::type>{}) {
              return E(std::ref(std::get<Index>(vs)));
            } else {
              return E(MapAccessStatus::key_type_mismatch);
            }
          }
        });

    static constexpr auto set_value_hash =
        make_sequential_table<size>([](auto&& index, auto& vs, const auto&& v) {
          using T = std::decay_t<decltype(index)>;
          if constexpr (invalid_input<T, size>()) {
            return MapAccessStatus::invalid_key;
          } else {
            constexpr std::size_t Index = index_map[T::value];
            if constexpr (std::is_same<std::tuple_element_t<
                                           Index, std::decay_t<decltype(vs)>>,
                                       std::decay_t<decltype(v)>>{}) {
              std::get<Index>(vs) = v;
              return MapAccessStatus::success;
            } else {
              return MapAccessStatus::key_type_mismatch;
            }
          }
        });

    static constexpr auto visitor_hash =
        make_sequential_table<size>([](auto&& index, auto& vs, auto&& visitor) {
          using T = std::decay_t<decltype(index)>;
          if constexpr (invalid_input<T, size>()) {
            return Expected<void, MapAccessStatus>(
                MapAccessStatus::invalid_key);
          } else {
            constexpr std::size_t Index = index_map[T::value];
            using R = std::result_of_t<decltype(visitor)(
                std::tuple_element_t<Index, std::decay_t<decltype(vs)>>)>;
            using E = Expected<R, MapAccessStatus>;
            if constexpr (!std::is_same<void, R>{}) {
              return E(visitor(std::get<Index>(vs)));
            } else {
              visitor(std::get<Index>(vs));
              return E();
            }
          }
        });
  };

  template<template<typename...> typename Hash, typename... Keys,
           typename... Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&,
                                    std::tuple<Values...>&& values) {
    return Map<Hash, std::tuple<Values...>, Keys...>{values};
  }

  template<typename... Keys, typename... Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&,
                                    std::tuple<Values...>&& values) {
    using ValueTuple = std::tuple<Values...>;
    return Map<SwitchTable, ValueTuple, Keys...>(
        std::forward<ValueTuple>(values));
  }

  template<typename... Keys, typename... Values>
  constexpr decltype(auto) make_map(std::pair<Keys, Values>&&... pairs) {
    auto result = split_pairs(std::forward<std::pair<Keys, Values>>(pairs)...);
    return make_map(std::move(result.first), std::move(result.second));
  }

}  // namespace petra
