#include <iostream>

#include "dispatch/array_jump_table.hpp"

template<size_t N>
struct printer {
  void operator()() const {
    std::cout << N << "\n";
  }
};

int main(int argc, char** argv) {
  constexpr unsigned limit = 4;
  constexpr auto table = dispatch::make_jump_table<printer, limit>();
  if (argc < 2) {
    return 255;
  }
  const unsigned int index = atoi(argv[1]);
  if (index >= limit) {
    return 255;
  }
  table[index]();
}
