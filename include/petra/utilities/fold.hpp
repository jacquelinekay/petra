#pragma once

#include <utility>

namespace petra {

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

  template <typename F, typename... Xs>
  constexpr auto fold_left(const F& f, Xs&&... xs) {
    auto result = (... >>= fold_wrapper<F, Xs>{f, xs});
    return result.state;
  }

  template <typename F, typename Init, std::size_t... I>
  constexpr auto fold_left(const F& f, Init&& init, std::index_sequence<I...>) {
    // auto result = (... >>= fold_wrapper<F, Xs>{f, xs});
    auto result = (fold_wrapper<F, Init>{f, init} >>= ...
        >>= fold_wrapper<F, std::integral_constant<std::size_t, I>>{
            f, std::integral_constant<std::size_t, I>{}});
    return result.state;
  }

}  // namespace petra
