// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/detail/macros.hpp"
#include "petra/indexed_bases.hpp"

namespace petra {

  // return a sequence of F's curried with integral_constant<0>, ..., N
  template<typename Result, typename F, typename T, T... Is>
  static constexpr auto
  make_sequenced_callbacks_helper(F& f, std::integer_sequence<T, Is...>&&) {
    auto curry_callback = [](auto& f, auto&& I) {
      return [&f](auto&&... args)
          PETRA_NOEXCEPT_FUNCTION_BODY(f(std::decay_t<decltype(I)>{}, args...));
    };
    return make_indexed_bases<Result>(
        curry_callback(f, std::integral_constant<T, Is>{})...);
  }

  template<auto N, typename Result, typename F>
  static constexpr auto make_sequenced_callbacks(F& f) {
    return make_sequenced_callbacks_helper<Result>(
        f, std::make_integer_sequence<decltype(N), N>{});
  }

  // Runtime-to-compile time map using indexed bases
  template<auto N, typename Result, typename F>
  struct BaseTable {
    constexpr BaseTable(F&& f) noexcept
        : callback(std::forward<F>(f)),
          indexed_bases(make_sequenced_callbacks<N, Result>(callback)) {}

    template<typename Integral, typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(
            indexed_bases(i, std::forward<Args>(args)...));

    F callback;
    using IndexedBaseT =
        decltype(make_sequenced_callbacks<N, Result>(callback));
    IndexedBaseT indexed_bases;
  };

  template<auto N, typename Result, typename F>
  constexpr decltype(auto) make_base_table(F&& f) {
    return BaseTable<N, Result, F>(std::forward<F>(f));
  }

}  // namespace petra
