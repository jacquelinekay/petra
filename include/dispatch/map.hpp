#pragma once

#include "dispatch/chd.hpp"
#include "dispatch/detail/index_map.hpp"
#include "dispatch/sequential_table.hpp"

namespace dispatch {
  /* A heterogenous map with frozen keys known at compile time.
   * Allows runtime lookup.
   * Types of values are fixed at compile time, but values are mutable.
   *
   * The interface is variant-like--considering naming this "VariantMap".
   * */

  template<template<typename...> typename Hash,
           typename Values, typename ...Keys>
  struct Map {
    // TODO: May need to make this a unique list
    // using ValueVariant = std::variant<Values...>;

    // TODO Error propagation if key is not in map

    // runtime key
    template<typename Key, typename Value>
    constexpr const Value& at(Key&& k) const {
      return std::get<index_lookup(k)>(values);
    }

    template<typename Key, typename Value>
    void insert(Key&& k, Value&& v) {
      std::get<index_lookup(k)>(values) = v;
    }


  private:
    template<typename Key>
    static constexpr std::size_t index_lookup(Key&& k) {
      return index_map[v_hash(key_hash(k))];
    }

    static constexpr std::size_t size = sizeof...(Keys);
    static constexpr auto key_hash = make_chd<Hash, Keys...>();

    Values values;

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(
            index_map_t{{0}}, 0);

    static constexpr auto value_map = [](auto&& index) {
      return std::decay_t<decltype(index)>::value;
    };
    static constexpr auto v_hash = make_sequential_table<size>(value_map);
  };

  template<template<typename...> typename Hash, typename... Keys, typename Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, Values&& values) {
    return Map<Hash, Values, Keys...>{values};
  }

  template<typename... Keys, typename Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, Values&& values) {
    return Map<SwitchTable, Values, Keys...>{values};
  }

}  // namespace dispatch
