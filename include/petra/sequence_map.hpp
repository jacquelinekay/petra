// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <array>
#include <cmath>
#include <utility>

#include "petra/sequential_table.hpp"
#include "petra/utilities.hpp"

namespace petra {

#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

  /* Given a sequence size known at compile time, map a std::array to a
   * std::integer_sequence
   * */
  template<typename F, typename Integral, Integral SeqSize, decltype(SeqSize) UpperBound>
  struct SequenceMap {
    static_assert(SeqSize > 0, "Cannot create a sequence map of size zero.");
    static_assert(UpperBound > 0, "Cannot create a sequence map with values bounded at 0.");

    constexpr SequenceMap(F&& f) : callback(f) {}

    static constexpr Integral Size = utilities::abs(SeqSize);
    using Array = std::array<Integral, Size>;

    template<Integral I>
    using int_c = std::integral_constant<Integral, I>;

    static constexpr Integral TotalSize = utilities::pow(UpperBound, Size);

    template<typename... Args>
    constexpr auto operator()(const Array& input, Args&&... args)
        PETRA_NOEXCEPT_FUNCTION_BODY(table(accumulate_sequence(input), callback,
                                           std::forward<Args>(args)...));
    // TODO Overflow checks
    static constexpr auto accumulate_sequence(const Array& input) noexcept {
      Integral total = 0;
      for (std::size_t i = 0; i < input.size(); ++i) {
        // hack
        if (input[i] >= UpperBound) {
          return TotalSize;
        }
        total += input[i] * utilities::pow(UpperBound, i);
      }
      return total;
    }

    static constexpr auto sequence_from_key(Integral Key, Integral Index) {
      Integral Base = utilities::pow(UpperBound, Index);
      return (Key / Base) % UpperBound;
    }

  private:

// TODO: noexcept specifier is a total hack
#define PETRA_BUILD_SEQUENCE_RETURNS(...) \
  cb(__VA_ARGS__, std::forward<Args>(args)...)

    template<Integral... Sequence, typename Key, typename Index,
             typename... Args>
    static constexpr auto build_sequence(Key&&, Index&&, F& cb, Args&&... args);

    template<Integral... Sequence, Integral Key,
             typename... Args>
    static constexpr auto build_sequence(int_c<Key>&&, int_c<Size - 1>&&, F& cb, Args&&... args)
    PETRA_NOEXCEPT_FUNCTION_BODY(PETRA_BUILD_SEQUENCE_RETURNS(
            std::integer_sequence<Integral, Sequence..., sequence_from_key(Key, Size - 1)>{}));

    template<Integral... Sequence, Integral Key, Integral Index,
             typename... Args>
    static constexpr auto build_sequence(int_c<Key>&&, int_c<Index>&&, F& cb, Args&&... args)
    PETRA_NOEXCEPT_FUNCTION_BODY(build_sequence<Sequence..., sequence_from_key(Key, Index)>(
            int_c<Key>{}, int_c<Index + static_cast<Integral>(1)>{}, cb, std::forward<Args>(args)...));

#define PETRA_MAP_TO_SEQUENCE_RETURNS(K)                                        \
  SequenceMap::build_sequence<>(int_c<K>{}, int_c<static_cast<Integral>(0)>{},                  \
      cb, std::forward<std::decay_t<decltype(args)>>(args)...)

#ifdef PETRA_ENABLE_CPP14
    struct map_to_seq_t {
      template<typename T, typename Cb, typename... Args>
      constexpr auto operator()(T&&, Cb& cb, Args&&... args)
      PETRA_NOEXCEPT_FUNCTION_BODY(PETRA_MAP_TO_SEQUENCE_RETURNS(T::value));

      template<typename Cb, typename... Args>
      constexpr auto operator()(InvalidInputError&& e, Cb& cb, Args&&... args)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          cb(std::forward<InvalidInputError>(e),
                  std::forward<std::decay_t<decltype(args)>>(args)...));

      template<typename T, typename Cb, typename... Args>
      constexpr auto operator()(T&&, Cb& cb, Args&&... args) const
      PETRA_NOEXCEPT_FUNCTION_BODY(PETRA_MAP_TO_SEQUENCE_RETURNS(T::value));

      template<typename Cb, typename... Args>
      constexpr auto operator()(InvalidInputError&& e, Cb& cb, Args&&... args) const
      PETRA_NOEXCEPT_FUNCTION_BODY(
          cb(std::forward<InvalidInputError>(e),
                  std::forward<std::decay_t<decltype(args)>>(args)...));
    };
    SequentialTable<map_to_seq_t, Integral, TotalSize> table{map_to_seq_t{}};
#else
    static constexpr auto map_to_seq = [](
        auto&& i, auto& cb,
        auto&&... args) noexcept(noexcept(PETRA_MAP_TO_SEQUENCE_RETURNS(0))) {
      using T = std::decay_t<decltype(i)>;
      if constexpr (utilities::is_error_type<T>()) {
        return cb(std::forward<T>(i),
                  std::forward<std::decay_t<decltype(args)>>(args)...);
      } else {
        return PETRA_MAP_TO_SEQUENCE_RETURNS(T::value);
      }
    };
    static constexpr auto table = make_sequential_table<Integral, TotalSize>(map_to_seq);
#endif  // PETRA_ENABLE_CPP14 

    F callback;
  };

  template<typename Integral, Integral SeqSize, Integral UpperBound, typename F>
  constexpr decltype(auto) make_sequence_map(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SequenceMap<F, Integral, SeqSize, UpperBound>(std::forward<F>(f)));

#ifndef PETRA_ENABLE_CPP14
  template<auto SeqSize, decltype(SeqSize) UpperBound, typename F>
  constexpr decltype(auto) make_sequence_map(F&& f)
      PETRA_NOEXCEPT_FUNCTION_BODY(
          SequenceMap<F, decltype(SeqSize), SeqSize, UpperBound>(std::forward<F>(f)));
#endif  // PETRA_ENABLE_CPP14 

#undef PETRA_BUILD_SEQUENCE_RETURNS
#undef PETRA_NOEXCEPT_FUNCTION_BODY
#undef PETRA_MAP_TO_SEQUENCE_RETURNS

}  // namespace petra
