// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <utility>
#include "petra/concepts.hpp"
#include "petra/utilities.hpp"

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/at.hpp>
#include <boost/hana/find.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/tuple.hpp>
#endif  // PETRA_ENABLE_CPP14

namespace petra {

#ifndef PETRA_ENABLE_CPP14
  template<auto I, typename T, T... Sequence, decltype(I)... Indices>
  static constexpr decltype(I) access_sequence_helper(
      const std::integer_sequence<T, Sequence...>&,
      const std::integer_sequence<decltype(I), Indices...>&) noexcept {
    return ((Indices == I ? Sequence : 0) + ...);
  }
#endif  // PETRA_ENABLE_CPP14

  // utility to access the Ith element of an index sequence
  template<std::size_t I, typename T, T... Sequence>
  static constexpr T
  access_sequence(const std::integer_sequence<T, Sequence...>& seq) noexcept {
    static_assert(I < sizeof...(Sequence), "Index exceeds size of sequence");
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    (void)seq;
    return hana::at_c<I>(hana::tuple_c<T, Sequence...>);
    // return hana::at<I>(hana::make_tuple(Sequence...));
#else
    return access_sequence_helper<I, T>(
        seq, std::make_integer_sequence<std::size_t, sizeof...(Sequence)>{});
#endif  // PETRA_ENABLE_CPP14
  }

  // Reverse operation: given an item in the sequence, return its index
  // TODO: These types should really be the same...
  template<PETRA_AUTO(In) I, typename T, T... Sequence>
  static constexpr decltype(I) map_to_index() noexcept {
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    return hana::find(hana::tuple_c<T, Sequence...>, hana::integral_constant<In, I>{});
#else
    using IndexT = decltype(I);
    return access_sequence_helper<I, T>(
        std::make_integer_sequence<IndexT, sizeof...(Sequence)>{},
        std::integer_sequence<T, Sequence...>{});
#endif  // PETRA_ENABLE_CPP14
  }

  template<PETRA_AUTO(In) I, decltype(I)... Sequence>
  static constexpr std::size_t
  map_to_index(std::integer_sequence<decltype(I), Sequence...>&&) noexcept {
#ifdef PETRA_ENABLE_CPP14
    return map_to_index<In, I, decltype(I), Sequence...>();
#else
    return map_to_index<I, decltype(I), Sequence...>();
#endif
  }

#ifndef PETRA_ENABLE_CPP14
  template<PETRA_AUTO(In) I, typename... T, decltype(I)... J>
  static constexpr std::size_t
  map_to_index_helper(std::tuple<T...>&& t,
                      std::integer_sequence<decltype(I), J...>&&) noexcept {
    return ((I == std::get<J>(t) ? J : 0) + ...);
  }

  template<PETRA_AUTO(In) I, typename... T>
  static constexpr std::size_t map_to_index(std::tuple<T...>&& t) noexcept {
    return map_to_index_helper<I>(
        t, std::make_integer_sequence<decltype(I), sizeof...(T)>{});
  }
#endif  // PETRA_ENABLE_CPP14

  template<PETRA_AUTO(In) I, decltype(I)... J>
  static constexpr auto append(std::integer_sequence<decltype(I), J...>) noexcept {
    return std::integer_sequence<decltype(I), J..., I>{};
  }

  template<typename T>
  static constexpr auto pop_front(const std::integer_sequence<T>&) noexcept {
    return std::integer_sequence<T>{};
  }

  template<PETRA_AUTO(In) I, decltype(I)... Is>
  static constexpr auto
  pop_front(const std::integer_sequence<decltype(I), I, Is...>&) noexcept {
    return std::make_pair(I, std::integer_sequence<decltype(I), Is...>{});
  }

  template<typename Integral>
  static constexpr bool in_sequence(const Integral&) noexcept {
    return false;
  }

  // check if the first element is in the following sequence
  template<typename Integral, typename T, typename... Ts>
  static constexpr bool in_sequence(const Integral& i, const T& t,
                                    const Ts&... ts) noexcept {
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    return hana::find(hana::make_tuple(t, ts...), i);
#else
    if constexpr (Comparable<Integral, T>()) {
      if (i == t) {
        return true;
      }
    }
    return in_sequence(i, ts...);
#endif  // PETRA_ENABLE_CPP14
  }

  template<typename Integral, Integral... J>
  static constexpr bool in_sequence(const Integral& I,
                                    std::integer_sequence<Integral, J...>) noexcept {
    return in_sequence(I, J...);
  }


#ifndef PETRA_ENABLE_CPP14
  template<typename T, T... Result>
  static constexpr auto
  make_unique_sequence(const std::integer_sequence<T, Result...>& result,
                       std::integer_sequence<T>) {
    return result;
  }

  // do we need to ensure that order is preserved?
  template<typename T, T... Result, T X, T... Sequence>
  static constexpr auto
  make_unique_sequence(const std::integer_sequence<T, Result...>&,
                       std::integer_sequence<T, X, Sequence...>) noexcept {
    if constexpr (in_sequence(X, Result...)) {
      return make_unique_sequence(std::integer_sequence<T, Result...>{},
                                  std::integer_sequence<T, Sequence...>{});
    } else {
      return make_unique_sequence(std::integer_sequence<T, Result..., X>{},
                                  std::integer_sequence<T, Sequence...>{});
    }
  }

  template<typename T, T... Sequence>
  static constexpr auto
  remove_repeats(const std::integer_sequence<T, Sequence...>& s) noexcept {
    return make_unique_sequence(std::integer_sequence<T>{}, s);
  }

  template<typename T, T... Sequence>
  static constexpr bool
  unique(const std::integer_sequence<T, Sequence...>& seq) noexcept {
    return seq.size() == remove_repeats(seq).size();
  }

  // precondition: J is a subset of I
  // Choose everything in I that is not in J
  template<typename T, T... I, T... J>
  static constexpr auto difference(std::integer_sequence<T, I...>,
                                   std::integer_sequence<T, J...>) noexcept {
    constexpr auto f = [](auto&& seq, auto&& i) {
      constexpr size_t index = std::decay_t<decltype(i)>::value;
      if constexpr (in_sequence(index, J...)) {
        return seq;
      } else {
        return append<index>(seq);
      }
    };
    return fold_left(f, std::integer_sequence<T>{},
                     std::integral_constant<T, I>{}...);
  }

  template<typename T, T... I, T... J>
  static constexpr bool disjoint(std::integer_sequence<T, I...>,
                                 std::integer_sequence<T, J...>) noexcept {
    return (!in_sequence(I, J...) && ...);
  }

#endif  // PETRA_ENABLE_CPP14

  template<typename T>
  static constexpr auto concatenate() noexcept {
    return std::integer_sequence<T>{};
  }

  template<typename T, T... I>
  constexpr auto concatenate(std::integer_sequence<T, I...> i) noexcept {
    return i;
  }

  template<typename T, T... I, T... J>
  static constexpr auto concatenate(std::integer_sequence<T, I...>,
                                    std::integer_sequence<T, J...>) noexcept {
    return std::integer_sequence<T, I..., J...>{};
  }

  template<typename T, T... I, T... J, typename... Ts>
  static constexpr auto concatenate(std::integer_sequence<T, I...>&& is,
                                    std::integer_sequence<T, J...>&& js,
                                    Ts&&... ts) noexcept {
    return concatenate(concatenate(is, js), ts...);
  }

}  // namespace petra
