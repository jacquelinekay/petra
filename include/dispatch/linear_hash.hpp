#pragma once

/* This is a fallback option for small input sets which CHD can't handle.
 * It has linear time performance, but for small sizes a constant time
 * algorithm is probably overkill anyway.
 * */

namespace dispatch {

  template<typename... Inputs>
  struct LinearHash {
    template<typename RuntimeType>
    constexpr decltype(auto) operator()(const RuntimeType& input) {
      return helper(input, std::index_sequence_for<Inputs...>{});
    }
  private:
    std::tuple<Inputs...> inputs;

    template<typename RuntimeType, std::size_t I, std::size_t ...J>
    constexpr decltype(auto) helper(const RuntimeType& input, std::index_sequence<<, J..>&&) {
      return ((std::get<I>(inputs) == input ? I : 0) + ...);
    }
  };

}  // namespace dispatch
