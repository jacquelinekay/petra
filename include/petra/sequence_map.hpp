// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <array>
#include <utility>

#include "petra/detail/macros.hpp"
#include "petra/sequential_table.hpp"
#include "petra/utilities.hpp"

namespace petra {

  /* Given a sequence size known at compile time, map a std::array to a
   * std::integer_sequence
   * */
  template<typename F, auto SeqSize, decltype(SeqSize) UpperBound>
  struct SequenceMap {
    static_assert(SeqSize > 0,
                  "Cannot instantiate a sequence map of size zero.");

    constexpr SequenceMap(F&& f) noexcept : callback(f) {}

    using Integral = decltype(SeqSize);
    static constexpr Integral Size = utilities::abs(SeqSize);
    using Array = std::array<Integral, Size>;

    template<typename... Args>
    constexpr auto operator()(const Array& input, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(
            seq_map(utilities::at(input, static_cast<Integral>(0)), input,
                    callback, std::forward<Args>(args)...));

  private:
    template<Integral CurrentIndex, Integral... Sequence>
    struct helper {
      template<Integral I, typename... Args>
      constexpr auto operator()(std::integral_constant<Integral, I>&&,
                                const Array& input, F& callback,
                                Args&&... args) const
          noexcept(noexcept(
              callback(std::integer_sequence<Integral, Sequence..., I>{},
                       std::forward<Args>(args)...))) {
        if constexpr (Size == 0) {
          return callback(std::integer_sequence<Integral>{},
                          std::forward<Args>(args)...);
        } else if constexpr (CurrentIndex == Size - 1) {
          static_assert(sizeof...(Sequence) + 1 == Size);
          return callback(std::integer_sequence<Integral, Sequence..., I>{},
                          std::forward<Args>(args)...);
        } else {
          static_assert(CurrentIndex < Size - 1);
          constexpr Integral NextIndex =
              CurrentIndex + static_cast<Integral>(1);
          return make_sequential_table<UpperBound>(
              helper<NextIndex, Sequence..., I>{})(
              input[NextIndex], input, callback, std::forward<Args>(args)...);
        }
      }

      template<typename... Args>
      constexpr auto operator()(InvalidInputError&& e, const Array&,
                                F& callback, Args&&... args) const
          PETRA_NOEXCEPT_FUNCTION_BODY(callback(
              std::forward<InvalidInputError>(e), std::forward<Args>(args)...));
    };

    static constexpr auto seq_map =
        make_sequential_table<UpperBound>(helper<static_cast<Integral>(0)>{});

    F callback;
  };

  template<auto SeqSize, decltype(SeqSize) UpperBound, typename F>
  constexpr decltype(auto) make_sequence_map(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SequenceMap<F, SeqSize, UpperBound>(std::forward<F>(f)));

}  // namespace petra
