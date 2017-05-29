#include <iostream>

#include "dispatch/array_jump_table.hpp"

template<std::size_t I>
struct printer {
  void operator()() {
    std::cout << I << "\n";
  }
};

int main(int argc, char** argv) {
  constexpr unsigned limit = 4;

  auto table = dispatch::make_tagged_jump_table<printer, limit>();
  if (argc < 2) {
    return 255;
  }
  const unsigned int index = atoi(argv[1]);
  if (index >= limit) {
    return 255;
  }
  table(index);
}
