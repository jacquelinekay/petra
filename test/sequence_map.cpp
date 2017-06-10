// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/sequence_map.hpp"
#include "utilities.hpp"
#include "petra/utilities/sequence.hpp"

#include <iostream>

static constexpr std::size_t sequence_size = 3;
static constexpr std::size_t upper_bound = 4;
using Array = std::array<std::size_t, sequence_size>;

struct callback {
  template<std::size_t... Sequence, std::size_t... Indices>
  auto operator()(std::index_sequence<Sequence...>&& seq, const Array& input,
                  std::index_sequence<Indices...>&&) noexcept {
    (PETRA_ASSERT(petra::access_sequence<Indices>(seq) == input[Indices]), ...);
  }

  template<typename... Args>
  auto operator()(petra::InvalidInputError&&, const Array& input,
                  Args&&...) noexcept {
    for (std::size_t i = 0; i < sequence_size; ++i) {
      if (input[i] >= upper_bound) {
        return;
      }
    }
    PETRA_ASSERT(false);
  }
};

int main() {
  {
    Array test{{1, 3, 2}};

    auto m = petra::make_sequence_map<sequence_size, upper_bound>(callback{});
    static_assert(
        noexcept(m(test, test, std::make_index_sequence<sequence_size>{})));
    m(test, test, std::make_index_sequence<sequence_size>{});
    // Error case
    test[0] = 4;
    m(test, test, std::make_index_sequence<sequence_size>{});
  }

  {
    auto callback_with_throw = [](auto&&...) {
      throw std::runtime_error("Catch this!");
    };
    auto m = petra::make_sequence_map<sequence_size, upper_bound>(
        callback_with_throw);
    static_assert(!noexcept(m(std::declval<Array>())));
  }

  return 0;
}
