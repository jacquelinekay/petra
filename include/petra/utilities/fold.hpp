// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>
#endif

namespace petra {

  template<typename F, typename X, typename X2, typename... Xs>
  static constexpr auto fold_left(const F& f, X&& x, X2&& x2, Xs&&... xs) {
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    return hana::fold(hana::make_tuple(std::forward<X2>(x2), std::forward<Xs>(xs)...),
          std::forward<X>(x), f);
#else
    if constexpr (sizeof...(Xs) == 0) {
      return f(x, x2);
    } else {
      return fold_left(f, f(x, x2), xs...);
    }
#endif  // PETRA_ENABLE_CPP14
  }

  template<typename F, typename Init, std::size_t I, std::size_t... J>
  static constexpr auto fold_left(const F& f, Init&& init,
                                  std::index_sequence<I, J...>) {
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    return hana::fold(hana::make_tuple(hana::size_c<I>, hana::size_c<J>...), init, f);
#else
    using IC = std::integral_constant<std::size_t, I>;
    if constexpr (sizeof...(J) == 0) {
      return f(init, IC{});
    } else {
      return fold_left(f, f(init, IC{}), std::index_sequence<J...>{});
    }
#endif  // PETRA_ENABLE_CPP14
  }

}  // namespace petra
