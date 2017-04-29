#pragma once

#include <utility>

template<size_t I, size_t ...Indices, size_t ...SequentialIndices>
constexpr unsigned access_sequence_helper(
    const std::index_sequence<Indices...>&, std::index_sequence<SequentialIndices...>&&) {
  return ((SequentialIndices == I ? Indices : 0) + ... );
}

// utility to access the Ith element of an index sequence
template<size_t I, size_t ...Indices>
constexpr unsigned access_sequence(const std::index_sequence<Indices...>& seq) {
  return access_sequence_helper<I>(seq, std::make_index_sequence<sizeof...(Indices)>{});
}

