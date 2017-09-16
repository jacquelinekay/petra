// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/string_map.hpp"
#include "petra/string_literal.hpp"

#include "utilities.hpp"

#include <unordered_map>

static constexpr size_t set_size = 10;

using namespace petra::literals;

int main() {
  {
    std::unordered_map<std::string, std::size_t> results;

    std::array<const char*, set_size> test_strings = {
        {"asdf", "qwerty", "quux", "int", "arguments", "foobar", "abcd", "badc",
         "foo", "oof"}};
    for (const auto& test_string : test_strings) {
      results.insert(std::make_pair(test_string, 0));
    }

    auto string_hash = petra::make_string_map(
        [&results](auto&& key) noexcept {
          if constexpr (petra::utilities::is_error_type<decltype(key)>()) {
            PETRA_ASSERT(false);
          } else {
            std::string key_s(key.get_value());
            PETRA_ASSERT(results.count(key_s) == 1);
            ++results[key_s];
          }
        },
        "asdf"_s, "qwerty"_s, "quux"_s, "int"_s, "arguments"_s, "foobar"_s,
        "abcd"_s, "badc"_s, "foo"_s, "oof"_s);

#ifdef __clang__
    static_assert(noexcept(string_hash(std::declval<const char*>())));
#endif
    for (const auto& s : test_strings) {
      string_hash(s);
      PETRA_ASSERT(results[s] == 1);
    }
  }

#ifdef __clang__
  {
    auto string_hash = petra::make_string_map(
        [](auto&&) { throw std::runtime_error("Rawr! I'm angry!"); }, "asdf"_s,
        "qwerty"_s, "quux"_s, "int"_s, "arguments"_s, "foobar"_s, "abcd"_s,
        "badc"_s, "foo"_s, "oof"_s);

    static_assert(!noexcept(string_hash(std::declval<const char*>())));
  }
#endif

  std::cout << "All string hash tests passed.\n";
  return 0;
}
