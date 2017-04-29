#include <iostream>

#include "dispatch/array_jump_table.hpp"

// example dispatch function object
template<size_t N>
struct printer {
  // The return type of operator() must not be dependent on the template parameters of this class.
  void operator()() const {
    std::cout << N << "\n";
  }
};

int main(int argc, char** argv) {
  constexpr unsigned limit = 4;
  constexpr auto table = make_jump_table<printer, limit>();
  if (argc < 2) {
    return 255;
  }
  const int index = atoi(argv[1]);
  if (index >= limit) {
    return 255;
  }
  table[index]();
}
