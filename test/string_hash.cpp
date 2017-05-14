#include "string_hash_utilities.hpp"

#include <iostream>

int main() {
  for (const auto result : results) {
    assert(result == 0);
  }

  constexpr auto string_constants = example_constants();

  auto test_strings = example_test_strings();

  constexpr auto string_dispatch_table = dispatch_table_from_tuple(string_constants);

  static_assert(unique_hashes(
      string_dispatch_table,
      string_constants,
      std::make_index_sequence<std::tuple_size<decltype(string_constants)>{}>{}));

  // Assert that the constexpr results of string_dispatch are the same as
  // the runtime hash
  assert(runtime_compiletime_match(string_dispatch_table, string_constants, test_strings));

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
