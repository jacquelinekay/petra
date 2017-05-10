#pragma once

#include <utility>

template<typename ...Strings>
constexpr unsigned get_max_string_length(Strings&&...) {
  return ([](auto max) {
    constexpr unsigned i = Strings::value().size();
    if constexpr (i > decltype(max){}) {
      return i;
    } else {
      return max;
    }
  }(std::integral_constant<unsigned, 0>{}), ...);
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
