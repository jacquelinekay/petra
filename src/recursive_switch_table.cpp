#include <iostream>

#include "dispatch/recursive_switch_table.hpp"

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    return 255;
  }
  const int index = atoi(argv[1]);

  auto switch_table = recursive_switch_table<printer, 100, 42, 8, 0>{printer{}};
  switch_table(index);
}
