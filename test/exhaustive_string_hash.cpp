#include "string_hash_utilities.hpp"

#include <algorithm>
#include <iostream>

// for ALL strings from size 0 to maxlength that are NOT
// in the string set, assert that they don't collide

template<size_t Length>
bool string_mutate(std::string& s, int index = 0) {
  if (index == Length) {
    return false;
  }
  if ((++s[index] % 128) == 0x0) {
    s[index] = 'a';
    return string_mutate<Length>(s, ++index);
  }
  return true;
}

template<typename Table, typename TestStrings, size_t... Length>
void hash_all_strings(const Table& table, const TestStrings& test_strings, std::index_sequence<Length...>&&) {
  ([&table, &test_strings]() {
    std::string test(Length, 'a'); // TODO: initialization
    do {
      for (const auto& key : test_strings) {
        // Check if test is a permutation of key
        std::string k(key);
        if (k == test) {
          continue;
        }
        /*
        std::reverse(k.begin(), k.end());
        if (k == test) {
          continue;
        }
        */
      }
      table(test.c_str());
    } while (string_mutate<Length>(test));
  }(), ...);
}

int main() {
  constexpr auto string_constants = example_constants();

  auto test_strings = example_test_strings();

  constexpr auto string_dispatch_table = dispatch_table_from_tuple(string_constants);

  constexpr auto max_length = max_string_length_tuple(string_constants);

  hash_all_strings(string_dispatch_table, test_strings, std::make_index_sequence<max_length>{});

  for (const auto result : results) {
    assert(result == 1);
  }
  std::cout << "All string hash tests passed.\n";

  return 0;
}
