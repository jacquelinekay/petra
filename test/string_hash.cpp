#include "dispatch/simple_string_hash.hpp"
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

  constexpr auto string_dispatch_table = make_string_dispatch<simple_string_hash, test>(
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

  // Assert that the constexpr results of string_dispatch are the same as
  // the runtime hash
  assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("asdf")) == string_dispatch_table.string_hash("asdf"));
  assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("qwerty")) == string_dispatch_table.string_hash("qwerty"));
  // assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("oof")) == string_dispatch_table.string_hash("oof"));
  assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("arguments")) == string_dispatch_table.string_hash("arguments"));
  assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("foo")) == string_dispatch_table.string_hash("foo"));
  assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("abcd")) == string_dispatch_table.string_hash("abcd"));
  // assert(string_dispatch_table.string_hash.hash(STRING_LITERAL("badc")) == string_dispatch_table.string_hash("badc"));

  for (const auto& s : test_strings) {
    std::cout << "Hash of " << s << ": ";
    std::cout << string_dispatch_table.string_hash(s) << "\n";
    std::cout << "Index of " << s << ": ";
    string_dispatch_table(s);
  }

  for (const auto result : results) {
    assert(result == 1);
  }

  std::cout << "All string hash tests passed.\n";
  return 0;
}
