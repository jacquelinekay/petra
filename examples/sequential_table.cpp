#include "petra/sequential_table.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Please enter an array size\n";
    return 255;
  }
  std::size_t x = std::stoul(argv[1]);
  if (x < 10) {
  }
  constexpr auto unwrap = petra::make_sequential_table<10>([](auto&& i){ return i(); }, 11);
  constexpr int y = unwrap(x);
  std::array<int, y> buckets = {{0}};

  (void)buckets;
}

