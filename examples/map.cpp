// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/map.hpp"
#include "petra/string_literal.hpp"

#include <iostream>
#include <vector>

using namespace petra::literals;

template<typename T>
using printable_t = decltype(std::declval<std::ostream&> << std::declval<T>());

template<typename T>
static constexpr bool Printable() {
  return petra::is_detected<printable_t, T>{};
}

int main() {
  auto example_map = petra::make_map(
      std::make_tuple("abc"_s, "foo"_s, "bar"_s, "qwerty"_s, "asdf"_s),
      std::make_tuple(0.1, 1, std::string("bar"), std::vector<int>{1, 2, 3},
                      std::make_tuple("hello", "world")));

  std::cout << "abc: " << *example_map.at<double>("abc").value() << "\n";
  std::cout << "foo: " << *example_map.at<int>("foo").value() << "\n";
  std::cout << "bar: " << *example_map.at<std::string>("bar").value() << "\n";
  example_map.insert("abc", 0.2);
  std::cout << "abc: " << *example_map.at<double>("abc").value() << "\n";

  constexpr std::size_t map_size = decltype(example_map)::size;

  for (std::size_t i = 0; i < map_size; ++i) {
    example_map.key_at(i).then([&example_map](auto&& k) {
      example_map.visit(k, [](const auto& x) {
        if constexpr (Printable<decltype(x)>()) {
          std::cout << x << std::endl;
        }
      });
    });
  }

  return 0;
}
