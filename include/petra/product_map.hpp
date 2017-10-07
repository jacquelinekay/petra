// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/switch_table.hpp"

namespace petra {

  template<typename T, auto OutputSize>
  struct ProductMap;

  // Product map is a multi-dimensional runtime to compile-time map.
  // At compile time it accepts a closed set of compile-time input Constants.
  // Its output range is the cartesian product of these values.
  // You can specify N things in the input set and an output size of M.
  template<typename... T, auto OutputSize>
  struct ProductMap<std::tuple<T...>, OutputSize> {
    static_assert((Constant<T>() && ...),
        "Input types to ProductMap must be constants.");

    template<typename ...InputT, typename = std::enable_if_t<(Constant<InputT>() && ...)>>
    constexpr auto operator(std::tuple<InputT...>&& input) {
      // Encode constant values to an integer index (hash?)
      // Map the integer index into a sequence map
      // Map to the constants 
    }
  };
}  // namespace petra
