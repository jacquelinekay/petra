#include <iostream>
#include "dispatch/string_hash.hpp"

#include "dispatch/utilities.hpp"

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }
};

int main() {
  constexpr dispatch::MinimalHash x(
      STRING_LITERAL("hello"),
      STRING_LITERAL("goodbye"),
      STRING_LITERAL("dog"),
      STRING_LITERAL("fish"),
      STRING_LITERAL("cat"));
  using Hash = decltype(x);

  std::cout << "hash(hello) => " << Hash::hash("hello") << "\n";
  std::cout << "hash(goodbye) => " << Hash::hash("goodbye") << "\n";
  std::cout << "hash(cat) => " << Hash::hash("cat") << "\n";
  std::cout << "hash(dog) => " << Hash::hash("dog") << "\n";
  std::cout << "hash(fish) => " << Hash::hash("fish") << "\n";
  /*
  std::cout << "hash(dog) => " << Hash::hash("dog") << "\n";
  constexpr auto dict = Hash::initialize_dictionary();

  constexpr auto print_tuple = [](auto&&... args) {
    (std::cout << ... << std::decay_t<decltype(args)>::value().data()) << "\n";
  };

  std::cout << "keys size: " << dict.first.size() << "\n";
  std::cout << "values size: " << tuple_size(dict.second) << "\n";
  std::apply(print_tuple, std::get<0>(dict.second));
  std::apply(print_tuple, std::get<1>(dict.second));
  std::apply(print_tuple, std::get<2>(dict.second));
  */

  // constexpr auto hash = dispatch::make_minimal_hash("foo", "bar");
  /*
  constexpr auto hash = make_string_hash(
    "foo",
    "bar",
    "baz",
    "quux"
  );

  std::array<const char*, 4> string_set = {"foo", "bar", "baz", "quux"};

  for (const auto& s : string_set) {
    std::cout << "hash(" << s << ") => " << hash(s) << "\n";
  }
  */
}
