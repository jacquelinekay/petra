#include "string_hash_utilities.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>

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

template<size_t I>
auto prepare_keyword_hashes(const std::array<const char*, I>& keywords) {
  std::array<std::set<char>, I> sets;
  for (int i = 0; i < I; ++i) {
    for (int j = 0; j < strlen(keywords[i]); ++j) {
      sets[i].insert(keywords[i][j]);
    }
  }
  return sets;
}

template<typename Table, typename TestStrings, size_t... Length>
void hash_all_strings(const Table& table, const TestStrings& test_strings, std::index_sequence<Length...>&&) {
  constexpr auto N = set_size;
  auto keyword_hashes = prepare_keyword_hashes<N>(test_strings);

  ([&table, &test_strings, &keyword_hashes]() {
    std::string test(Length, 'a'); // TODO: initialization
    do {
      bool skip = false;
      for (int j = 0; j < N; ++j) {
        const auto key = test_strings[j];
        if (key == test.c_str()) {
          // don't try to hash this string
          skip = true;
        }
      }
      if (!skip) {
        std::cout << test << "\n";
        table(test.c_str());
      }
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
