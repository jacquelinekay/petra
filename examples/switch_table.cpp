// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include <iostream>

#include "petra/switch_table.hpp"

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }

  void operator()(petra::InvalidInputError&&) const {
    std::cout << "Received a value that wasn't in the input set\n";
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    return 255;
  }
  const int index = atoi(argv[1]);

  auto switch_table =
      petra::make_switch_table<std::size_t, 100, 42, 8, 0>(printer{});
  switch_table(index);
}
