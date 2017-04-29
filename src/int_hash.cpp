#include "dispatch/int_hash.hpp"

template<size_t N>
struct printer {
  void operator()() const {
    std::cout << N << "\n";
  }
};

int main(int argc, char** argv) {
  constexpr auto table = unsequenced_jump_table<printer, 0, 42, 8>();
  if (argc < 2) {
    return 255;
  }
  const int index = atoi(argv[1]);

  table(index);
}
