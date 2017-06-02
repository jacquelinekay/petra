// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/chd.hpp"
#include "petra/sequential_table.hpp"
#include "petra/detail/index_map.hpp"
#include "petra/utilities/tuple.hpp"

#include <type_traits>

namespace petra {
  /* A heterogenous map with frozen keys known at compile time.
   * Allows runtime lookup.
   * Types of values are fixed at compile time, but values are mutable.
   *
   * The interface is variant-like--considering naming this "VariantMap".
   * */

  template<template<typename...> typename Hash,
           typename Values, typename ...Keys>
  struct Map {
    Map(Values&& v) : values(v) { }

    template<typename Value, typename Key>
    constexpr Value const* at(Key&& k) const {
      Value const* v;
      set_pointer_hash(key_hash(k), values, v);
      return v;
    }

    // returns a read-only pointer to the location where v was inserted,
    // or nullptr if lookup failed
    template<typename Key, typename Value>
    constexpr const Value* insert(Key&& k, Value&& v) {
      return set_value_hash(key_hash(k), values, std::forward<Value>(v));
    }

    template<typename Key, typename Visitor>
    constexpr decltype(auto) visit(
        Key&& k,
        Visitor&& visitor) {
      return visitor_hash(
          key_hash(k),
          values,
          std::forward<Visitor>(visitor),
          [](){
            throw std::runtime_error("Got invalid index in petra::map::visit");
          });
    }

    template<typename Key, typename Visitor, typename E>
    constexpr decltype(auto) visit(
        Key&& k,
        Visitor&& visitor,
        E&& error_callback) {
      return visitor_hash(
          key_hash(k),
          values,
          std::forward<Visitor>(visitor),
          std::forward<E>(error_callback));
    }

    // template<typename K>
    constexpr decltype(auto) key_at(std::size_t i) const {
      return make_sequential_table<size>(
        [](auto&& i, auto&& k) {
          constexpr std::size_t I = std::decay_t<decltype(i)>::value;
          if constexpr (I >= size) {
            return "";
          } else {
            return std::get<I>(k).data();
          }
        }
      )(i, keys);
    }

    static constexpr std::size_t size = sizeof...(Keys);

  private:
    static constexpr auto key_hash = make_chd<Hash, Keys...>();

    std::tuple<Keys...> keys;
    Values values;

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(index_map_t{});

    static constexpr auto set_pointer_hash = make_sequential_table<size>(
        [](auto&& index, const auto& vs, auto*& ptr) {
          constexpr std::size_t I = std::decay_t<decltype(index)>::value;
          if constexpr (I >= size) {
            ptr = nullptr;
            return;
          } else {
            constexpr std::size_t Index = index_map[I];
            if constexpr (std::is_same<
                std::tuple_element_t<Index, std::decay_t<decltype(vs)>>,
                std::decay_t<decltype(*ptr)>>{}) {
              ptr = &std::get<Index>(vs);
            } else {
              ptr = nullptr;
            }
          }
        });

    static constexpr auto set_value_hash = make_sequential_table<size>(
        [](auto&& index, auto& vs, const auto&& v) -> std::add_pointer_t<decltype(v)> {
          constexpr std::size_t I = std::decay_t<decltype(index)>::value;
          if constexpr (I >= size) {
            return nullptr;
          } else {
            constexpr std::size_t Index = index_map[I];
            if constexpr (std::is_same<std::tuple_element_t<Index, std::decay_t<decltype(vs)>>,
                                       std::decay_t<decltype(v)>>{}) {
              std::get<Index>(vs) = v;
              return &std::get<Index>(vs);
            } else {
              return nullptr;
            }
          }
        });

    static constexpr auto visitor_hash = make_sequential_table<size>(
        [](auto&& index, auto& vs, auto&& visitor, auto&& error_callback) {
          constexpr std::size_t I = std::decay_t<decltype(index)>::value;
          if constexpr (I >= size) {
            return error_callback();
          } else {
            return visitor(std::get<index_map[I]>(vs));
          }
        });
  };


  template<template<typename...> typename Hash, typename... Keys, typename... Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, std::tuple<Values...>&& values) {
    return Map<Hash, std::tuple<Values...>, Keys...>{values};
  }

  template<typename... Keys, typename... Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, std::tuple<Values...>&& values) {
    using ValueTuple = std::tuple<Values...>;
    return Map<SwitchTable, ValueTuple, Keys...>(std::forward<ValueTuple>(values));
  }

  template<typename... Keys, typename... Values>
  constexpr decltype(auto) make_map(std::pair<Keys, Values>&&... pairs) {
    auto result = split_pairs(std::forward<std::pair<Keys, Values>>(pairs)...);
    return make_map(std::move(result.first), std::move(result.second));
  }

}  // namespace petra
