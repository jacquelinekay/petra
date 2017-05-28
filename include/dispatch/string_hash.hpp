#pragma once

#include "dispatch/detail/string_hash.hpp"

namespace dispatch {

  template<typename ...Strings>
  struct MinimalHash {
    // TODO: enable_if a simpler hash when the string set has 4 or fewer words
    static constexpr std::size_t set_size = sizeof...(Strings);
    static_assert(set_size > 4);

    static constexpr auto intermediate_hash = detail::construct_hash<Strings...>();

    static constexpr auto hash(const char* str){
    // TODO: len(G) != set_size, so this differs from the Python impl
    std::size_t key = detail::distinct_hash(0, str, set_size);
    const auto [status, d] = intermediate_hash(key);
    switch(status) {
      case detail::hash_status::Unique:
        return d;
      case detail::hash_status::Collision:
        return detail::distinct_hash(d, str, set_size);  // TODO: len(V)?
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
