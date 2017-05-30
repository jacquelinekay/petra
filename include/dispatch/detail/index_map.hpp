#pragma once

#include <array>

namespace dispatch {
namespace detail {
  template<typename IndexMap, std::size_t X, std::size_t ...HashSequence>
  static constexpr auto init_index_map(IndexMap cur_map, std::size_t i) {
    cur_map[X] = i;
    if constexpr (sizeof...(HashSequence) == 0) {
      return cur_map;
    } else {
      return init_index_map<IndexMap, HashSequence...>(cur_map, i + 1);
    }
  }
}  // namespace detail
}  // namespace dispatch
