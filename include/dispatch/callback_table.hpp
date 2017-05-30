#pragma once

#include "dispatch/chd.hpp"
#include "dispatch/sequential_table.hpp"

namespace dispatch {
  /* A callback table with heterogeneous keys known at compile time.
   * The return type of the callbacks must be uniform.
   * */

  template<template<typename...> typename Hash, typename Callbacks, typename ...Keys>
  struct CallbackTable {
    CallbackTable(Callbacks&& c) : callbacks(c) { }

    template<typename Key, typename ...Args>
    decltype(auto) trigger(Key&& key, Args&&... args) {
      return v_hash(key_hash(key), callbacks, std::forward<Args>(args)...);
    }

  private:
    Callbacks callbacks;

    static constexpr std::size_t size = sizeof...(Keys);
    static constexpr auto key_hash = make_chd<Hash, Keys...>();

    static constexpr auto callback_map = [](auto&& index, auto&& c, auto&&... args) {
      constexpr std::size_t I = std::decay_t<decltype(index)>::value;
      if constexpr (I < std::tuple_size<Callbacks>{}) {
        return std::get<I>(c)(args...);
      }
    };

    static constexpr auto v_hash = make_sequential_table<size>(callback_map);
  };

  template<typename Fs, typename ...Keys>
  decltype(auto) make_callback_table(
      std::tuple<Keys...>&&, Fs&& callbacks) {
    return CallbackTable<SwitchTable, Fs, Keys...>(std::forward<Fs>(callbacks));
  }

}  // namespace dispatch
