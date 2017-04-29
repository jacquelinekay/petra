/* given a set of constexpr strings,
 * map those strings to integers
 *
 * build a function that accepts const char*'s and retrieves the associated integer
 *
 * compare to the performance of a std::unordered_map<string, int>
 *
 */
#include <iostream>

#include "dispatch/string_hash.hpp"

template<size_t N>
struct printer {
  void operator()() const {
    std::cout << N << "\n";
  }
};

int main() {
  constexpr unsigned max = get_max_string_length(
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );
  static_assert(max == 4);

  constexpr auto hash = prepare_string_hash(
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );

  std::array<const char*, 4> string_set = {"foo", "bar", "baz", "quux"};

  for (const auto& s : string_set) {
    std::cout << "hash(" << s << ") => " << hash(s) << "\n";
  }

  constexpr auto string_dispatch_table = make_string_dispatch<printer>(
    STRING_LITERAL("foo"),
    STRING_LITERAL("bar"),
    STRING_LITERAL("baz"),
    STRING_LITERAL("quux")
  );

  for (const auto& s : string_set) {
    string_dispatch_table(s);
  }
}
