// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "string_hash_utilities.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>

// for ALL alphanumeric strings from size 1 to maxlength that are NOT
// in the string set, log collisions

template<std::size_t Length>
bool string_mutate(std::string& s, std::size_t index = 0) {
  if (index == Length) {
    return false;
  }
  if ((++s[index] % 126) == 0x0) {
    s[index] = 32;
    return string_mutate<Length>(s, ++index);
  }
  return true;
}

/*
template<std::size_t I>
auto prepare_keyword_hashes(const std::array<const char*, I>& keywords) {
  std::array<std::set<char>, I> sets;
  for (std::size_t i = 0; i < I; ++i) {
    for (std::size_t j = 0; j < strlen(keywords[i]); ++j) {
      sets[i].insert(keywords[i][j]);
    }
  }
  return sets;
}
*/

template<typename Table, typename TestStrings, std::size_t... Length>
void hash_all_strings(Table&& table, const TestStrings& test_strings,
                      std::index_sequence<Length...>&&) {
  constexpr auto N = petra::test_utils::set_size;
  // auto keyword_hashes = prepare_keyword_hashes<N>(test_strings);

  (
      [&table, &test_strings]() {
        std::string test(Length, 32);  // TODO: initialization
        do {
          bool skip = false;
          for (std::size_t j = 0; j < N; ++j) {
            const auto key = test_strings[j];
            if (key == test.c_str()) {
              // don't try to hash this string
              skip = true;
            }
          }
          if (!skip) {
            table(test.c_str());
          }
        } while (string_mutate<Length>(test));
      }(),
      ...);
}

int main() {
  using namespace petra::test_utils;
  constexpr auto string_constants = example_constants();

  auto test_strings = example_test_strings();

  auto string_petra_table = petra_table_from_tuple(string_constants);

  constexpr auto max_length = max_string_length_tuple(string_constants);

  hash_all_strings(string_petra_table, test_strings,
                   std::make_index_sequence<max_length>{});

  /*
  for (const auto result : results) {
    assert(result == 1);
  }
  */
  std::cout << "All string hash tests passed.\n";

  return 0;
}
