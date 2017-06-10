// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/chd.hpp"
#include "petra/concepts.hpp"
#include "petra/detail/index_map.hpp"
#include "petra/sequential_table.hpp"

namespace petra {
  /* A callback table with heterogeneous keys known at compile time.
   * The return type of the callbacks must be uniform.
   * */
  template<template<typename...> typename Hash, typename Callbacks,
           typename Keys, typename ErrorCallback = void*>
  struct CallbackTable;

  template<template<typename...> typename Hash, typename Callbacks,
           typename... Keys, typename ErrorCallback>
  struct CallbackTable<Hash, Callbacks, std::tuple<Keys...>, ErrorCallback> {
    constexpr CallbackTable(Callbacks&& c) : callbacks(c) {}

    constexpr CallbackTable(Callbacks&& c, ErrorCallback&& e)
        : callbacks(c), error_callback(e) {}

    template<typename Key, typename... Args>
    constexpr decltype(auto) trigger(const Key& key, Args&&... args) {
      return v_hash(key_hash(key), callbacks, error_callback,
                    std::forward<Args>(args)...);
    }

  private:
    Callbacks callbacks;
    ErrorCallback error_callback;

    static constexpr std::size_t size = sizeof...(Keys);
    static constexpr auto key_hash = make_chd<Hash, Keys...>();
    using index_map_t = std::array<std::size_t, size>;

    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, key_hash(Keys{})...>(index_map_t{});

    static_assert(size == std::tuple_size<Callbacks>{});

    static constexpr auto callback_map = [](auto&& index, auto&& c, auto&& e,
                                            auto&&... args) {
      using T = std::decay_t<decltype(index)>;
      if constexpr (invalid_input<T, size>()) {
        if constexpr (!std::is_same<ErrorCallback, void*>{}) {
          return e(args...);
        }
      } else {
        constexpr std::size_t I = T::value;
        static_assert(index_map[I] < std::tuple_size<Callbacks>{});
        return std::get<index_map[I]>(c)(args...);
      }
    };

    static constexpr auto v_hash = make_sequential_table<size>(callback_map);
  };

  template<typename... Keys, typename... Fs>
  constexpr decltype(auto) make_callback_table(std::tuple<Keys...>&&,
                                               std::tuple<Fs...>&& callbacks) {
    using FTuple = std::tuple<Fs...>;
    return CallbackTable<SwitchTable, FTuple, std::tuple<Keys...>>(
        std::forward<FTuple>(callbacks));
  }

  template<typename... Keys, typename... Fs, typename E,
           typename = std::enable_if_t<!TupleAccess<E>()>>
  constexpr decltype(auto) make_callback_table(std::tuple<Keys...>&&,
                                               std::tuple<Fs...>&& callbacks,
                                               E&& e) {
    using FTuple = std::tuple<Fs...>;
    return CallbackTable<SwitchTable, FTuple, std::tuple<Keys...>, E>(
        std::forward<FTuple>(callbacks), std::forward<E>(e));
  }

  template<typename... Pairs,
           typename = std::enable_if_t<(PairAccess<Pairs>() && ...)>>
  constexpr decltype(auto) make_callback_table(std::tuple<Pairs...>&& pairs) {
    constexpr auto f = [](Pairs&&... args) {
      return make_callback_table(std::forward<Pairs>(args)...);
    };
    return std::apply(f, std::forward<std::tuple<Pairs...>>(pairs));
  }

  template<typename E, typename... Pairs,
           typename = std::enable_if_t<(PairAccess<Pairs>() && ...)
                                       && !PairAccess<E>()>>
  constexpr decltype(auto) make_callback_table(std::tuple<Pairs...>&& pairs,
                                               E&& e) {
    auto f = [&e](Pairs&&... args) {
      auto result = split_pairs(std::forward<Pairs>(args)...);
      using K = decltype(result.first);
      using F = decltype(result.second);
      return CallbackTable<SwitchTable, F, K, E>(std::forward<F>(result.second),
                                                 std::forward<E>(e));
    };
    return std::apply(f, std::forward<std::tuple<Pairs...>>(pairs));
  }

  template<typename... Keys, typename... Fs>
  constexpr decltype(auto) make_callback_table(std::pair<Keys, Fs>&&... pairs) {
    auto result = split_pairs(std::forward<std::pair<Keys, Fs>>(pairs)...);
    using K = decltype(result.first);
    using F = decltype(result.second);
    return CallbackTable<SwitchTable, F, K>(std::move(result.second));
  }

}  // namespace petra
