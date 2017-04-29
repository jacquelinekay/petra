// Macro based approach
#include <iostream>

#include "dispatch/switch_macro.hpp"

template<size_t N>
struct printer {
  void operator()() const {
    std::cout << N << "\n";
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    return 255;
  }
  const int index = atoi(argv[1]);

  auto switch_table = MAKE_SWITCH_JUMP_TABLE(printer, 100, 42, 8, 0);
  switch_table(index);
}
