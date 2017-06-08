// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include <iostream>

// #include "petra/chd.hpp"
#include "petra/string_literal.hpp"
#include "petra/string_map.hpp"
#include "petra/utilities.hpp"

using namespace petra::literals;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Please enter a test token!\n";
    return 255;
  }

  const char* input = argv[1];

  auto callback = [](auto&& token) {
    std::cout << "hash value: " << std::decay_t<decltype(token)>::value() << "\n";
  };

  auto map = petra::make_string_map(callback, "hello"_s, "goodbye"_s, "dog"_s, "fish"_s, "cat"_s);

  map(input);

  return 0;
}
