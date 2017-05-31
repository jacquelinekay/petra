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
    Map(Values&& v) : values(v) { }
    // TODO: May need to make this a unique list
    // using ValueVariant = std::variant<Values...>;

    // TODO Error propagation if key is not in map

    // runtime key
    // use expected instead?
    template<typename Value, typename Key>
    constexpr const Value* at(Key&& k) const {
      const Value* v;
      set_pointer(std::forward<Key>(k), values, v);
      return v;
    }

    // TODO: Error handling
    template<typename Key, typename Value>
    void insert(Key&& k, Value&& v) {
      set_value_hash(key_hash(k), values, std::forward<Value>(v));
    }


  private:
    template<typename Key, typename Value>
    static constexpr void set_pointer(Key&& k, const Values& vs, Value*& vptr) {
      return set_pointer_hash(key_hash(k), vs, vptr);
    }

    static constexpr std::size_t size = sizeof...(Keys);
    static constexpr auto key_hash = make_chd<Hash, Keys...>();

    Values values;

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(
            index_map_t{{0}}, 0);

    static constexpr auto set_pointer_l = [](auto&& index, const auto& vs, auto& ptr) {
      constexpr std::size_t I = std::decay_t<decltype(index)>::value;
      if constexpr (I >= size) {
        ptr = nullptr;
        return;
      } else {
        constexpr std::size_t Index = index_map[I];
        if constexpr (std::is_same<std::tuple_element_t<Index, std::decay_t<decltype(vs)>>,
                                   std::decay_t<decltype(*ptr)>>{}) {
          ptr = &std::get<Index>(vs);
        } else {
          ptr = nullptr;
        }
      }
    };

    // TODO: error handling
    static constexpr auto set_value = [](auto&& index, auto& vs, const auto&& v) {
      constexpr std::size_t I = std::decay_t<decltype(index)>::value;
      if constexpr (I >= size) {
        return;
      } else {
        constexpr std::size_t Index = index_map[I];
        if constexpr (std::is_same<std::tuple_element_t<Index, std::decay_t<decltype(vs)>>,
                                   std::decay_t<decltype(v)>>{}) {
          std::get<Index>(vs) = v;
        }
      }
    };

    static constexpr auto set_pointer_hash = make_sequential_table<size>(set_pointer_l);
    static constexpr auto set_value_hash = make_sequential_table<size>(set_value);
  };


  template<template<typename...> typename Hash, typename... Keys, typename Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, Values&& values) {
    return Map<Hash, Values, Keys...>{values};
  }

  template<typename... Keys, typename Values>
  constexpr decltype(auto) make_map(std::tuple<Keys...>&&, Values&& values) {
    return Map<SwitchTable, Values, Keys...>(std::forward<Values>(values));
  }

}  // namespace dispatch
