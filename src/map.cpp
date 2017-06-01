#include "dispatch/map.hpp"
#include "dispatch/string_literal.hpp"

#include <iostream>
#include <vector>

using namespace dispatch::literals;

template<typename T>
using printable_t = decltype(std::cout << std::declval<T>());

template<typename T>
static constexpr bool Printable() {
  return dispatch::is_detected<printable_t, T>{};
}

int main() {
  auto example_map = dispatch::make_map(
    std::make_tuple(
      "abc"_s,
      "foo"_s,
      "bar"_s,
      "qwerty"_s,
      "asdf"_s
    ),
    std::make_tuple(
      0.1,
      1,
      std::string("bar"),
      std::vector<int>{1, 2, 3},
      std::make_tuple("hello", "world")
    )
  );

  std::cout << "abc: " << *example_map.at<double>("abc") << "\n";
  std::cout << "foo: " << *example_map.at<int>("foo") << "\n";
  std::cout << "bar: " << *example_map.at<std::string>("bar") << "\n";
  example_map.insert("abc", 0.2);
  std::cout << "abc: " << *example_map.at<double>("abc") << "\n";

  constexpr std::size_t map_size = decltype(example_map)::size;

  for (std::size_t i = 0; i < map_size; ++i) {
    example_map.visit(
        example_map.key_at(i),
        [](const auto& x){
          if constexpr (Printable<decltype(x)>()) {
            std::cout << x << std::endl;
          }
        });
  }

  return 0;
}
