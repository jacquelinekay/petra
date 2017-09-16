// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include <iostream>

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
    using T = std::decay_t<decltype(token)>;
    if constexpr (petra::utilities::is_error_type<T>()) {
      std::cout << "Could not hash input to a matching key.\n";
    } else {
      std::cout << "hash value: " << T::value << "\n";
    }
  };

  auto map = petra::make_string_map(callback, "hello"_s, "goodbye"_s, "dog"_s,
                                    "fish"_s, "cat"_s);

  map(input);

  return 0;
}
