// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/chd.hpp"
#include "petra/detail/index_map.hpp"
#include "petra/sequential_table.hpp"

namespace petra {

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  /* Provides runtime to compile-time string mapping for strings
   *
   * Interface:
   *
   * make_string_map(std::make_tuple(inputs...),
   *   // where str is a compile-time string corresponding to what triggered the
   * callback
   *   [](auto&& str) {
   *   }
   * );
   *
   * */
  template<typename F, typename ErrorString, typename... Inputs>
  struct StringMap {
    constexpr StringMap(F&& f) : callback(f) {}

    template<typename... Args>
    constexpr decltype(auto) operator()(const char* input, Args&&... args)
    PETRA_NOEXCEPT_FUNCTION_BODY(
        matched_callback(chd(input), this->callback, std::forward<Args>(args)...));

  private:
    F callback;
    static constexpr auto inputs = std::make_tuple(Inputs{}...);
    static constexpr auto chd = make_chd(Inputs{}...);
    static constexpr std::size_t size = sizeof...(Inputs);

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, chd(Inputs{})...>(index_map_t{});

    static constexpr auto matched_callback =
      petra::make_sequential_table<size>([](auto&& index, auto& callback, auto&&... args)
      noexcept((noexcept(callback(Inputs{}, args...)) &&...) && noexcept(callback(ErrorString{}, args...)))
      {
        constexpr std::size_t I = std::decay_t<decltype(index)>::value;
        if constexpr (I < size) {
          return callback(std::get<index_map[I]>(inputs), args...);
        } else {
          return callback(ErrorString{}, args...);
        }
      });
  };

  template<typename F, typename... Inputs>
  static constexpr auto make_string_map(F&& f, Inputs&&...) {
    // The default value of "ErrorString" is an empty string
    return StringMap<F, string_literal<char>, Inputs...>(std::forward<F>(f));
  }

  template<typename F, typename... Inputs, typename ErrorString>
  static constexpr auto make_string_map(F&& f, std::tuple<Inputs...>&&,
                                        ErrorString&&) {
    return StringMap<F, ErrorString, Inputs...>(std::forward<F>(f));
  }

#undef PETRA_NOEXCEPT_FUNCTION_BODY

}  // namespace petra
