// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/callback_table.hpp"
#include "utilities.hpp"

using namespace petra::literals;

int main() {
  constexpr auto keys =
      std::make_tuple("asdf"_s, "qwerty"_s, "quux"_s, "int"_s, "arguments"_s,
                      "foobar"_s, "abcd"_s, "badc"_s, "foo"_s, "oof"_s);
  constexpr std::size_t set_size = std::tuple_size<decltype(keys)>{};

  std::array<const char*, set_size> test_strings = {
      {"asdf", "qwerty", "quux", "int", "arguments", "foobar", "abcd", "badc",
       "foo", "oof"}};
  std::array<std::size_t, set_size> results = {{0}};

  auto make_callback_pairs = [&test_strings, &results](auto&&... args) {
    auto f = [&test_strings, &results](auto&& result, const auto& key) {
      return append(result, std::make_pair(
                                key, [&key, &test_strings, &results](auto&& i) {
                                  PETRA_ASSERT(key == test_strings[i]);
                                  ++results[i];
                                }));
    };
    return fold_left(f, std::make_tuple(), args...);
  };

  bool errored = false;

  auto table =
      petra::make_callback_table(std::apply(make_callback_pairs, keys),
                                 [&errored](auto&&...) { errored = true; });

  for (std::size_t i = 0; i < set_size; ++i) {
    table.trigger(test_strings[i], i);
    PETRA_ASSERT(results[i] == 1);
  }

  return 0;
}
