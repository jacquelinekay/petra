// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>

namespace petra {
  // #ifdef __GNUC__

  template<typename F, typename X, typename X2, typename... Xs>
  static constexpr auto fold_left(const F& f, X&& x, X2&& x2, Xs&&... xs) {
    if constexpr (sizeof...(Xs) == 0) {
      return f(x, x2);
    } else {
      return fold_left(f, f(x, x2), xs...);
    }
  }

  template<typename F, typename Init, std::size_t I, std::size_t... J>
  static constexpr auto fold_left(const F& f, Init&& init,
                                  std::index_sequence<I, J...>) {
    using IC = std::integral_constant<std::size_t, I>;
    if constexpr (sizeof...(J) == 0) {
      return f(init, IC{});
    } else {
      return fold_left(f, f(init, IC{}), std::index_sequence<J...>{});
    }
  }

  /*
#else
  template<typename F, typename X>
  struct fold_wrapper {
    F f;
    X state;

    template<typename Arg>
    constexpr auto operator>>=(Arg&& arg) {
      auto result = f(state, arg.state);
      return fold_wrapper<F, decltype(result)>{f, result};
    }
  };

  template<typename F, typename... Xs>
  constexpr auto fold_left(const F& f, Xs&&... xs) {
    auto result = (... >>= fold_wrapper<F, Xs>{f, xs});
    return result.state;
  }

  template<typename F, typename Init, std::size_t... I>
  constexpr auto fold_left(const F& f, Init&& init, std::index_sequence<I...>) {
    auto result = (fold_wrapper<F, Init>{f, init} >>= ... >>=
                   fold_wrapper<F, std::integral_constant<std::size_t, I>>{
                       f, std::integral_constant<std::size_t, I>{}});
    return result.state;
  }
#endif
*/

}  // namespace petra
