// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <array>
#include <cmath>
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
        "Cannot create a sequence map of size zero.");
    static_assert(UpperBound > 0,
        "Cannot create a sequence map with values bounded at 0.");

    constexpr SequenceMap(F&& f) : callback(f) {}

    using Integral = decltype(SeqSize);
    static constexpr Integral Size = utilities::abs(SeqSize);
    using Array = std::array<Integral, Size>;

    static constexpr Integral TotalSize = utilities::pow(UpperBound, Size);

    template<typename... Args>
    constexpr auto operator()(const Array& input, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(table(accumulate_sequence(input), callback,
                                           std::forward<Args>(args)...));

  private:
    // TODO Overflow checks
    static constexpr auto accumulate_sequence(const Array& input) noexcept {
      Integral total = 0;
      for (std::size_t i = 0; i < input.size(); ++i) {
        total += input[i] * utilities::pow(UpperBound, i);
      }
      return total;
    }

#define PETRA_BUILD_SEQUENCE_RETURNS() \
  cb(std::integer_sequence<Integral, Sequence...>{}, std::forward<Args>(args)...)

    template<Integral Key, Integral Index, Integral... Sequence,
             typename... Args>
    static constexpr auto build_sequence(F& cb, Args&&... args)
    PETRA_NOEXCEPT_CHECK(PETRA_BUILD_SEQUENCE_RETURNS()) {
      if constexpr (Index == SeqSize) {
        static_assert(sizeof...(Sequence) == SeqSize);
        return PETRA_BUILD_SEQUENCE_RETURNS();
      } else {
        constexpr Integral Base = utilities::pow(UpperBound, Index);
        static_assert(Base != 0);
        constexpr Integral Next = (Key / Base) % UpperBound;
        return build_sequence<Key, Index + static_cast<Integral>(1), Next,
                              Sequence...>(cb, std::forward<Args>(args)...);
      }
    }


#define PETRA_MAP_TO_SEQUENCE_RETURNS(K)                                        \
  SequenceMap::build_sequence<K, static_cast<Integral>(0)>(                  \
      cb, std::forward<std::decay_t<decltype(args)>>(args)...)

    static constexpr auto map_to_seq = [](
        auto&& i, auto& cb,
        auto&&... args) PETRA_NOEXCEPT_CHECK(PETRA_MAP_TO_SEQUENCE_RETURNS(0)) {
      using T = std::decay_t<decltype(i)>;
      if constexpr (utilities::is_error_type<T>()) {
        return cb(std::forward<T>(i),
                  std::forward<std::decay_t<decltype(args)>>(args)...);
      } else {
        return PETRA_MAP_TO_SEQUENCE_RETURNS(T::value);
      }
    };

    static constexpr auto table = make_sequential_table<TotalSize>(map_to_seq);

    F callback;
  };

  template<auto SeqSize, decltype(SeqSize) UpperBound, typename F>
  constexpr decltype(auto) make_sequence_map(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SequenceMap<F, SeqSize, UpperBound>(std::forward<F>(f)));

#undef PETRA_BUILD_SEQUENCE_RETURNS
#undef PETRA_MAP_TO_SEQUENCE_RETURNS

}  // namespace petra
