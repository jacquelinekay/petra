// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

namespace petra {
  namespace detail {
    template<typename T, T... Pack>
    struct string_literal;

    template<typename T, T... P>
    struct compare {
      template<size_t... I>
      static constexpr bool apply(const char* v, std::index_sequence<I...>) {
        return ((P == v[I]) && ...);
      }
      template<typename S, S... Q>
      static constexpr bool apply() {
        return ((P == Q) && ...);
      }
    };

    template<typename D, std::size_t... I>
    constexpr decltype(auto) from_string_literal(D&&,
                                                 std::index_sequence<I...>&&) {
      return string_literal<char, D::char_at(I)...>{};
    }

  }  // namespace detail
}  // namespace petra
