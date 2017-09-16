// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/detail/macros.hpp"
#include "petra/switch_table.hpp"

namespace petra {

  /* Provides runtime to compile-time string mapping for enum types
   * */
  template<typename F, typename Enum, Enum... Values>
  struct EnumMap {
    using Integral = std::underlying_type_t<Enum>;

    template<Enum E>
    using enum_constant = std::integral_constant<Enum, E>;

    constexpr EnumMap(F&& f) : callback(f) {}

    template<typename... Args>
    constexpr decltype(auto) operator()(Enum input, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(cast_to_enum(static_cast<Integral>(input),
                                                  callback,
                                                  std::forward<Args>(args)...));

  private:
    F callback;
    static constexpr std::size_t size = sizeof...(Values);

    struct forward_callbacks {
      template<typename Callback, typename... Args>
      constexpr auto operator()(InvalidInputError&& input, Callback& cb,
                                Args&&... args) const
          PETRA_NOEXCEPT_FUNCTION_BODY(
              cb(std::forward<InvalidInputError>(input),
                 std::forward<Args>(args)...));

      template<auto I, typename Callback, typename... Args>
      constexpr auto operator()(std::integral_constant<decltype(I), I>&&,
                                Callback& cb, Args&&... args) const
          PETRA_NOEXCEPT_FUNCTION_BODY(cb(enum_constant<static_cast<Enum>(I)>{},
                                          std::forward<Args>(args)...));
    };

    static constexpr auto cast_to_enum =
        petra::make_switch_table<Integral, static_cast<Integral>(Values)...>(
            forward_callbacks{});
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
