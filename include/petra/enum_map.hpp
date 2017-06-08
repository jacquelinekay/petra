// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/switch_table.hpp"

namespace petra {
  /* Provides runtime to compile-time string mapping for enum types
   * */
  template<typename F, typename Enum, Enum... Values>
  struct EnumMap {
    using Integral = std::underlying_type_t<Enum>;
    constexpr EnumMap(F&& f) : callback(f) {}

    template<typename... Args>
    constexpr decltype(auto) operator()(Enum input, Args&&... args) {
      return cast_to_enum(static_cast<Integral>(input), callback,
                          std::forward<Args>(args)...);
    }

  private:
    F callback;
    static constexpr std::size_t size = sizeof...(Values);
    static constexpr auto cast_to_enum = petra::make_switch_table<
        Integral, static_cast<Integral>(Values)...>([](auto&& integer, auto& cb,
                                                       auto&&... args) {
      using Input = std::decay_t<decltype(integer)>;
      constexpr std::integral_constant<Enum, static_cast<Enum>(Input::value)> e;
      return cb(e, std::forward<decltype(args)>(args)...);
    });
  };

  template<typename Enum, Enum... Values, typename F>
  static constexpr auto make_enum_map(F&& f) {
    return EnumMap<F, Enum, Values...>(std::forward<F>(f));
  }

  template<typename Enum, Enum... Values, typename F>
  static constexpr auto
  make_enum_map(F&& f, std::integral_constant<Enum, Values>&&...) {
    return EnumMap<F, Enum, Values...>(std::forward<F>(f));
  }

}  // namespace petra
