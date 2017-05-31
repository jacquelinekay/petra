#include "dispatch/callback_table.hpp"
#include "utilities.hpp"

using namespace dispatch::literals;

int main() {
  constexpr auto keys = std::make_tuple(
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
  constexpr std::size_t set_size = std::tuple_size<decltype(keys)>{};

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

  auto make_callback_pairs = [&test_strings](auto&&... args) {
    auto f = [&test_strings](auto&& result, const auto& key) {
      return append(result, std::make_pair(
          key,
          [&key, &test_strings](auto&& i){
            DISPATCH_ASSERT(key == test_strings[i]);
          }));
    };
    return fold_left(f, std::make_tuple(), args...);
  };

  auto table = dispatch::make_callback_table(std::apply(make_callback_pairs, keys));
  for (std::size_t i = 0; i < set_size; ++i) {
    table.trigger(test_strings[i], i);
  }
  return 0;
}
