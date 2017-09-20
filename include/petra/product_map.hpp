// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/switch_table.hpp"

namespace petra {

  template<typename T>
  struct ProductMap;

  // Inputs: A set of constant templates in their order in the tuple
  // and a set of ranges for those values
  // do I need a range concept?
  template<typename... T>
  struct ProductMap<std::tuple<T...>> {
    template<>
    constexpr auto operator(std::tuple<T...>&& input) {}
  };
}  // namespace petra
