#include "dispatch/naive_string_hash.hpp"
#include "dispatch/string_dispatch.hpp"

#include <iostream>

static constexpr size_t set_size = 9;

using namespace dispatch::literals;

struct test {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) {
    std::cout << N << "\n";
    ++results[N];
    assert(results[N] == 1);
  }
  std::array<int, set_size> results = {{0}};
};

int main() {
  std::array<const char*, set_size > test_strings = {{
    "asdf",
    "qwerty",
    "quux",
    "int",
    "arguments",
    "foobar",
    "abcd",
    "badc",
    "foo",
    // "oof"
  }};

  auto string_dispatch_table = dispatch::make_naive_string_hash(
    test{},
    "asdf"_s,
    "qwerty"_s,
    "quux"_s,
    "int"_s,
    "arguments"_s,
    "foobar"_s,
    "abcd"_s,
    "badc"_s,
    "foo"_s
    // "oof"_s
  );

  for (const auto& s : test_strings) {
    string_dispatch_table(s);
  }

  std::cout << "All string hash tests passed.\n";
  return 0;
}
