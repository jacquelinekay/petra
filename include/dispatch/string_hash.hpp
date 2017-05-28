#pragma once

#include "dispatch/detail/string_hash.hpp"

namespace dispatch {

  template<typename IntermediateHash, typename ...Inputs>
  struct MinimalHash {
    // TODO: enable_if a simpler hash when the string set has 4 or fewer words
    static constexpr std::size_t set_size = sizeof...(Inputs);
    static_assert(set_size > 4);

    static constexpr auto second_hash =
        detail::construct_hash<IntermediateHash, Inputs...>();

    template<typename RuntimeType>
    static constexpr auto hash(const RuntimeType& input){
      // TODO: len(G) != set_size, so this differs from the Python impl
      std::size_t key = detail::distinct_hash(0, input, set_size);
      const auto [status, d] = second_hash(key);
      switch(status) {
        case detail::hash_status::Unique:
          return d;
        case detail::hash_status::Collision:
          return detail::distinct_hash(d, input, set_size);  // TODO: len(V)?
        case detail::hash_status::Empty:
        default:
          return set_size;
      }
    }

  };

  // TODO: Factories for all kinds o' strings

  template<typename... Args>
  static constexpr auto make_minimal_hash(Args&&... args) {
    return MinimalHash<std::decay_t<decltype(args)>...>{};
  }

}  // namespace dispatch
