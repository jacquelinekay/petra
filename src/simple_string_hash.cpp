/* given a set of constexpr strings,
 * map those strings to integers
 *
 * build a function that accepts const char*'s and retrieves the associated integer
 *
 * compare to the performance of a std::unordered_map<string, int>
 *
 */
#include <iostream>

#include "dispatch/simple_string_hash.hpp"
#include "dispatch/string_dispatch.hpp"

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }
};

int main() {
  constexpr unsigned max = dispatch::utilities::max_string_length(
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );
  static_assert(max == 4);

  constexpr auto hash = dispatch::make_simple_string_hash(
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );

  std::array<const char*, 4> string_set = {{"foo", "bar", "baz", "quux"}};

  for (const auto& s : string_set) {
    std::cout << "hash(" << s << ") => " << hash(s) << "\n";
  }

  auto string_dispatch_table = dispatch::make_string_dispatch<dispatch::simple_string_hash>(
    printer{},
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );

  for (const auto& s : string_set) {
    string_dispatch_table(s);
  }
}
