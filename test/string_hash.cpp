#include "dispatch/chd.hpp"
#include "dispatch/string_literal.hpp"

#include "utilities.hpp"

#include <iostream>

static constexpr size_t set_size = 10;

using namespace dispatch::literals;

int main() {

  std::array<int, set_size> results = {{0}};

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
    "oof"
  }};

  auto string_hash = dispatch::make_chd(
    "asdf"_s,
    "qwerty"_s,
    "quux"_s,
    "int"_s,
    "arguments"_s,
    "foobar"_s,
    "abcd"_s,
    "badc"_s,
    "foo"_s,
    "oof"_s
  );

  for (const auto& s : test_strings) {
    std::size_t N = string_hash(s);
    std::cout << N << "\n";
    ++results[N];
    DISPATCH_ASSERT(results[N] == 1);
  }

  std::cout << "All string hash tests passed.\n";
  return 0;
}
