#pragma once

#include <utility>
#include <tuple>

template<size_t Max, typename Cur, typename ...Strings>
constexpr auto max_string_length_recursive(const std::tuple<Cur, Strings...>& str) {
  constexpr unsigned Size = std::decay_t<Cur>::value().size();
  if constexpr (Size > Max) {
    if constexpr (sizeof...(Strings) > 0) {
      return max_string_length_recursive<Size>(std::make_tuple(Strings{}...));
    } else {
      return Size;
    }
  } else {
    if constexpr (sizeof...(Strings) > 0) {
      return max_string_length_recursive<Max>(std::make_tuple(Strings{}...));
    } else {
      return Max;
    }
  }
}

template<typename ...Strings>
constexpr auto max_string_length(const Strings&... strs) {
  return max_string_length_recursive<0>(std::make_tuple(strs...));
}

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
