// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include "petra/concepts.hpp"

#include <tuple>
#include <utility>

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>
#endif

/* This is a fallback option for small input sets which CHD can't handle.
 * It has linear runtime complexity, but for small sizes (less than 4) this may
 * be acceptable.
 * */

namespace petra {

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  template<typename... Inputs>
  struct LinearHash {
    template<typename RuntimeType>
    static constexpr decltype(auto) hash(RuntimeType&& input)
        PETRA_NOEXCEPT_FUNCTION_BODY(
            helper(std::forward<RuntimeType>(input),
                   std::index_sequence_for<Inputs...>{}));

    template<typename RuntimeType>
    constexpr decltype(auto) operator()(RuntimeType&& input) const
        PETRA_NOEXCEPT_FUNCTION_BODY(hash(std::forward<RuntimeType>(input)));

  private:
    static constexpr auto inputs = std::make_tuple(Inputs{}...);

    template<typename RuntimeType>
    static constexpr decltype(auto) helper(RuntimeType&&,
                                           std::index_sequence<>&&) noexcept {
      return sizeof...(Inputs);
    }

    template<typename RuntimeType, std::size_t I, std::size_t... J>
    static constexpr decltype(auto)
    helper(RuntimeType&& input, std::index_sequence<I, J...>&&) noexcept {
      if (std::get<I>(inputs) == input) {
        return I;
      } else {
        return helper(input, std::index_sequence<J...>{});
      }
    }
  };

#ifdef PETRA_ENABLE_CPP14
  struct constant_concept_helper {
    template<typename Elem>
    constexpr auto operator()(bool output, Elem&&) const {
      return output && Constant<typename std::decay_t<Elem>::type>();
    }
  };

  template<typename... Inputs, typename = std::enable_if_t<boost::hana::fold(
                                   boost::hana::tuple_t<Inputs...>, true,
                                   constant_concept_helper{})>>
#else
  template<typename... Inputs,
           typename = std::enable_if_t<(Constant<Inputs>() && ...)>>
#endif  // PETRA_ENABLE_CPP14
  constexpr decltype(auto) make_linear_hash(Inputs&&...) {
    return LinearHash<Inputs...>{};
  }

#undef PETRA_NOEXCEPT_FUNCTION_BODY

}  // namespace petra
