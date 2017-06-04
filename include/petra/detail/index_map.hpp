// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <array>

namespace petra {
  namespace detail {
    template<typename IndexMap, std::size_t X, std::size_t... HashSequence>
    static constexpr auto init_index_map(IndexMap cur_map, std::size_t i) {
      cur_map[X] = i;
      if constexpr (sizeof...(HashSequence) == 0) {
        return cur_map;
      } else {
        return init_index_map<IndexMap, HashSequence...>(cur_map, i + 1);
      }
    }

    template<typename IndexMap, std::size_t... HashSequence>
    static constexpr auto init_index_map(IndexMap cur_map) {
      static_assert(sizeof...(HashSequence) == std::tuple_size<IndexMap>{});

      return init_index_map<IndexMap, HashSequence...>(cur_map, 0);
    }
  }  // namespace detail
}  // namespace petra
