// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <array>
#include <utility>

#include "petra/detail/macros.hpp"

namespace petra {

  // Ab Wilson's technique, adapted from:
  // https://godbolt.org/g/ZcBLPn
  template<typename Result, typename... Fs>
  struct IndexedBases : Fs... {
    // TODO: Can we get around this result type limitation?
    template<typename... Args>
    using Handler = Result (IndexedBases::*)(Args...) const noexcept;

    template<typename... Args>
    using Handlers = std::array<Handler<Args...>, sizeof...(Fs)>;

    constexpr IndexedBases(Fs&&... fs) noexcept : Fs(std::forward<Fs>(fs))... {}

    // for now, bounds checking intentionally left out
    template<typename Integral, typename... Args>
    constexpr auto operator()(Integral i, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(
            (this->*(handlers<Args...>()[i]))(std::forward<Args>(args)...));

    template<typename... Args>
    static constexpr Handlers<Args...> handlers() noexcept {
      return {{&Fs::operator()...}};
    }
  };

  template<typename Result, typename... Fs>
  constexpr auto make_indexed_bases(Fs&&... fs) noexcept {
    return IndexedBases<Result, Fs...>(std::forward<Fs>(fs)...);
  }

}  // namespace petra
