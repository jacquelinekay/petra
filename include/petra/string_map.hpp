// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/chd.hpp"
#include "petra/detail/index_map.hpp"
#include "petra/sequential_table.hpp"

namespace petra {

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
  template<typename F, typename Error, typename... Inputs>
  struct StringMap {
    constexpr StringMap(F&& f) : callback(f) {}

    template<typename... Args>
    constexpr decltype(auto) operator()(const char* input, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(matched_callback(
            chd(input), this->callback, std::forward<Args>(args)...));

  private:
    F callback;
    static constexpr auto inputs = std::make_tuple(Inputs{}...);
    static constexpr auto chd = make_chd(Inputs{}...);
    static constexpr std::size_t size = sizeof...(Inputs);

    using index_map_t = std::array<std::size_t, size>;
    static constexpr index_map_t index_map =
        detail::init_index_map<index_map_t, chd(Inputs{})...>(index_map_t{});

    static constexpr auto matched_callback =
        petra::make_sequential_table<size>([](
            auto&& index, auto& callback,
            auto&&... args) noexcept((noexcept(callback(Inputs{}, args...))
                                      && ...)
                                     && noexcept(callback(Error{}, args...))) {
          using T = std::decay_t<decltype(index)>;
          if constexpr (invalid_input<T, size>()) {
            return callback(Error{}, args...);
          } else {
            constexpr std::size_t I = index_map[T::value];
            return callback(std::get<I>(inputs), args...);
          }
        });
  };

  template<typename F, typename... Inputs>
  static constexpr auto make_string_map(F&& f, Inputs&&...) {
    // The default type of "Error" is InvalidInputError
    return StringMap<F, InvalidInputError, Inputs...>(std::forward<F>(f));
  }

  template<typename F, typename... Inputs, typename Error>
  static constexpr auto make_string_map(F&& f, std::tuple<Inputs...>&&,
                                        Error&&) {
    return StringMap<F, Error, Inputs...>(std::forward<F>(f));
  }

}  // namespace petra
