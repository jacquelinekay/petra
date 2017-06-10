// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/sequence_map.hpp"
#include "petra/utilities.hpp"

#include <iostream>

static constexpr std::size_t sequence_size = 3;
static constexpr std::size_t upper_bound = 4;

struct callback {
  template<std::size_t... I>
  auto operator()(std::integer_sequence<std::size_t, I...>&&) {
    (std::cout << ... << I);
    std::cout << "\n";
  }

  auto operator()(petra::InvalidInputError&&) {
    // Do stuff
    std::cout << "ERROR\n";
  }
};

int main(int argc, char** argv) {
  std::array<std::size_t, sequence_size> test{{0}};
  if (static_cast<std::size_t>(argc) - 1 > sequence_size) {
    std::cout << "Sorry, we can only take " << sequence_size
              << " integers from the command line.\n";
    return 255;
  }

  for (int i = 1; i < argc; ++i) { test[i - 1] = std::stoul(argv[i]); }

  auto m = petra::make_sequence_map<sequence_size, upper_bound>(callback{});
  m(test);

  return 0;
}
