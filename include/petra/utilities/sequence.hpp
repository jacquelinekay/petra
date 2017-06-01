#pragma once

#include <utility>
#include "petra/concepts.hpp"
#include "petra/utilities.hpp"

namespace petra {

  template<size_t I, size_t ...Sequence, size_t ...Indices>
  constexpr unsigned access_sequence_helper(
      const std::index_sequence<Sequence...>&, const std::index_sequence<Indices...>&) {
    return ((Indices == I ? Sequence : 0) + ... );
  }

  // utility to access the Ith element of an index sequence
  template<size_t I, size_t ...Sequence>
  constexpr unsigned access_sequence(const std::index_sequence<Sequence...>& seq) {
    return access_sequence_helper<I>(
      seq, std::make_index_sequence<sizeof...(Sequence)>{});
  }

  // Reverse operation: given an item in the sequence, return its index
  template<size_t I, size_t ...Sequence>
  constexpr unsigned map_to_index() {
    return access_sequence_helper<I>(
      std::make_index_sequence<sizeof...(Sequence)>{}, std::index_sequence<Sequence...>{});
  }
  template<size_t I, size_t ...Sequence>
  constexpr unsigned map_to_index(std::index_sequence<Sequence...>) {
    return map_to_index<I, Sequence...>();
  }

  template<size_t I, typename T, size_t... J>
  constexpr unsigned map_to_index_helper(T&& t, std::index_sequence<J...>) {
    return ((I == std::get<J>(t) ? J : 0) + ...);
  }

  template<size_t I, typename T>
  constexpr unsigned map_to_index(T&& t) {
    return map_to_index_helper<I>(t, std::make_index_sequence<std::tuple_size<std::decay_t<T>>{}>{});
  }

  template<size_t I, size_t... J>
  static constexpr auto append(std::index_sequence<J...>) {
    return std::index_sequence<J..., I>{};
  }

  constexpr auto pop_front(const std::index_sequence<>&) {
    return std::index_sequence<>{};
  }

  template<std::size_t I, std::size_t... Is>
  constexpr auto pop_front(const std::index_sequence<I, Is...>&) {
    return std::make_pair(I, std::index_sequence<Is...>{});
  }

  template<typename Integral>
  constexpr bool in_sequence(const Integral&) {
    return false;
  }
  // check if the first element is in the following sequence
  template<typename Integral, typename T, typename... Ts>
  constexpr bool in_sequence(const Integral& i, const T& t, const Ts&... ts) {
    if constexpr (Comparable<Integral, T>()) {
      if (i == t) {
        return true;
      }
    }
    return in_sequence(i, ts...);
  }

  template<typename Integral, std::size_t... J>
  constexpr bool in_sequence(const Integral& I, std::index_sequence<J...>) {
    return in_sequence(I, J...);
  }

  template<std::size_t... Result>
  static constexpr auto make_unique_sequence(
      const std::index_sequence<Result...>& result, std::index_sequence<>) {
    return result;
  }

  // do we need to ensure that order is preserved?
  template<std::size_t... Result, std::size_t X, std::size_t... Sequence>
  constexpr auto make_unique_sequence(
      const std::index_sequence<Result...>&, std::index_sequence<X, Sequence...>) {
    if constexpr (in_sequence(X, Result...)) {
      return make_unique_sequence(
          std::index_sequence<Result...>{}, std::index_sequence<Sequence...>{});
    } else {
      return make_unique_sequence(
          std::index_sequence<Result..., X>{}, std::index_sequence<Sequence...>{});
    }
  }

  template<std::size_t... Sequence>
  static constexpr auto remove_repeats(const std::index_sequence<Sequence...>& s) {
    return make_unique_sequence(std::index_sequence<>{}, s);
  }

  template<std::size_t... Sequence>
  static constexpr bool unique(const std::index_sequence<Sequence...>& seq) {
    return seq.size() == remove_repeats(seq).size();
  }

  constexpr auto concatenate() {
    return std::index_sequence<>{};
  }

  template<size_t... I>
  constexpr auto concatenate(std::index_sequence<I...> i) {
    return i;
  }

  template<size_t... I, size_t... J>
  constexpr auto concatenate(std::index_sequence<I...>, std::index_sequence<J...>) {
    return std::index_sequence<I..., J...>{};
  }

  template<size_t... I, size_t... J, typename... T>
  constexpr auto concatenate(std::index_sequence<I...>&& is, std::index_sequence<J...>&& js, T&&... ts) {
    return concatenate(concatenate(is, js), ts...);
  }

  // precondition: J is a subset of I
  // Choose everything in I that is not in J
  template<size_t... I, size_t... J>
  constexpr auto difference(std::index_sequence<I...>, std::index_sequence<J...>) {
    constexpr auto f = [](auto&& seq, auto&& i) {
      constexpr size_t index = std::decay_t<decltype(i)>::value;
      if constexpr (in_sequence(index, J...)) {
        return seq;
      } else {
        return append<index>(seq);
      }
    };
    return fold_left(f, std::index_sequence<>{}, std::integral_constant<std::size_t, I>{}...);
  }

  template<size_t... I, size_t... J>
  constexpr bool disjoint(std::index_sequence<I...>, std::index_sequence<J...>) {
    return (!in_sequence(I, J...) && ...);
  }

}  // namespace petra
