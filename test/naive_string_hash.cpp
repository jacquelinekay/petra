#include "dispatch/naive_string_hash.hpp"
#include "dispatch/string_dispatch.hpp"

#include <iostream>

static constexpr size_t set_size = 9;

static std::array<int, set_size> results = {0};

template<size_t N>
struct test {
  void operator()() const {
    std::cout << N << "\n";
    ++results[N];
  }
};

int main() {
  for (const auto result : results) {
    assert(result == 0);
  }

  std::array<const char*, set_size > test_strings = {
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
  };

  constexpr auto string_dispatch_table = make_naive_string_hash<test>(
    STRING_LITERAL("asdf"),
    STRING_LITERAL("qwerty"),
    STRING_LITERAL("quux"),
    STRING_LITERAL("int"),
    STRING_LITERAL("arguments"),
    STRING_LITERAL("foobar"),
    STRING_LITERAL("abcd"),
    STRING_LITERAL("badc"),
    STRING_LITERAL("foo")
    // STRING_LITERAL("oof")
  );

  for (const auto& s : test_strings) {
    string_dispatch_table(s);
  }


  for (const auto result : results) {
    assert(result == 1);
  }

  std::cout << "All string hash tests passed.\n";
  return 0;
}
