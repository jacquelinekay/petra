// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/detail/chd.hpp"
#include "petra/utilities/sequence.hpp"

namespace petra {

  template<template<typename...> typename IntermediateHash, typename... Inputs>
  struct CHDHash {
    static constexpr std::size_t set_size = sizeof...(Inputs);
    static constexpr bool use_fallback = set_size <= 4;

    template<typename RuntimeType>
    static constexpr std::size_t hash(const RuntimeType& input) noexcept {
      if constexpr (!use_fallback) {
        using adl::chd;
        std::size_t key = chd(0, input, set_size, adl::chd_tag{});
        const auto [status, d] = second_hash(key);
        switch (status) {
          case detail::hash_status::Unique: return d;
          case detail::hash_status::Collision:
            return chd(d, input, set_size, adl::chd_tag{});
          case detail::hash_status::Empty:
          default: return set_size;
        }
      } else {
        return second_hash(input);
      }
    }

    template<typename RuntimeType>
    constexpr auto operator()(const RuntimeType& input) const noexcept {
      return hash(input);
    }

  private:
    static constexpr auto second_hash =
        detail::construct_hash<IntermediateHash, Inputs...>();

    static constexpr bool no_collisions() noexcept {
      return unique(std::index_sequence<hash(Inputs{})...>{});
    }

    static_assert(no_collisions());
  };

  template<template<typename...> typename Hash, typename... Args,
           typename = std::enable_if_t<(Constant<Args>() && ...)>>
  static constexpr auto make_chd() noexcept {
    return CHDHash<Hash, Args...>{};
  }

  template<typename... Args,
           typename = std::enable_if_t<(Constant<Args>() && ...)>>
  static constexpr auto make_chd() noexcept {
    return CHDHash<SwitchTable, Args...>{};
  }

  template<
      template<typename...> typename Hash, typename A, typename... Args,
      typename = std::enable_if_t<Constant<A>() && (Constant<Args>() && ...)>>
  static constexpr auto make_chd(A&&, Args&&...) noexcept {
    return CHDHash<Hash, std::decay_t<A>, std::decay_t<Args>...>{};
  }

  template<
      typename A, typename... Args,
      typename = std::enable_if_t<Constant<A>() && (Constant<Args>() && ...)>>
  static constexpr auto make_chd(A&&, Args&&...) noexcept {
    return CHDHash<SwitchTable, std::decay_t<A>, std::decay_t<Args>...>{};
  }

}  // namespace petra
