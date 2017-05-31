#pragma once

#include "dispatch/detail/index_map.hpp"
#include "dispatch/chd.hpp"
#include "dispatch/sequential_table.hpp"

namespace dispatch {
  /* A callback table with heterogeneous keys known at compile time.
   * The return type of the callbacks must be uniform.
   * */
  template<template<typename...> typename Hash, typename Callbacks, typename Keys, typename ErrorCallback = void*>
  struct CallbackTable;

  template<template<typename...> typename Hash, typename Callbacks, typename ...Keys, typename ErrorCallback>
  struct CallbackTable<Hash, Callbacks, std::tuple<Keys...>, ErrorCallback> {

    constexpr CallbackTable(Callbacks&& c) : callbacks(c) { }

    constexpr CallbackTable(Callbacks&& c, ErrorCallback&& e)
      : callbacks(c), error_callback(e) { }

    template<typename Key, typename ...Args>
    constexpr decltype(auto) trigger(Key&& key, Args&&... args) {
      return v_hash(key_hash(key), callbacks, error_callback, std::forward<Args>(args)...);
    }

  private:
    Callbacks callbacks;
    ErrorCallback error_callback;

    static constexpr std::size_t size = sizeof...(Keys);
    static constexpr auto key_hash = make_chd<Hash, Keys...>();
    using index_map_t = std::array<std::size_t, size>;

    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(index_map_t{});

    static constexpr auto callback_map = [](auto&& index, auto&& c, auto&& e, auto&&... args) {
      constexpr std::size_t I = std::decay_t<decltype(index)>::value;
      if constexpr (I < std::tuple_size<Callbacks>{}) {
        return std::get<index_map[I]>(c)(args...);
      } else if constexpr (!std::is_same<ErrorCallback, void*>{}) {
        return e(args...);
      }
    };

    static constexpr auto v_hash = make_sequential_table<size>(callback_map);
  };

  template<typename Keys, typename Fs>
  constexpr decltype(auto) make_callback_table(Keys&&, Fs&& callbacks) {
    return CallbackTable<SwitchTable, Fs, Keys>(std::forward<Fs>(callbacks));
  }

  template<typename Keys, typename Fs, typename E>
  constexpr decltype(auto) make_callback_table(Keys&&, Fs&& callbacks, E&& e) {
    return CallbackTable<SwitchTable, Fs, Keys>(
        std::forward<Fs>(callbacks), std::forward<E>(e));
  }

  template<typename ...Pairs>
  constexpr decltype(auto) make_callback_table(Pairs&&...);

  template<typename ...Keys, typename ...Fs>
  constexpr decltype(auto) make_callback_table(std::pair<Keys, Fs>&&... pairs) {
    auto result = split_pairs(std::forward<std::pair<Keys, Fs>>(pairs)...);
    using K = decltype(result.first);
    using F = decltype(result.second);
    return CallbackTable<SwitchTable, F, K>(std::forward<F>(result.second));
  }

  template<typename ...Pairs>
  constexpr decltype(auto) make_callback_table(std::tuple<Pairs...>&& pairs) {
    constexpr auto f = [](Pairs&&... args) {
      return make_callback_table(std::forward<Pairs>(args)...);
    };
    return std::apply(f, std::forward<std::tuple<Pairs...>>(pairs));
  }

}  // namespace dispatch
