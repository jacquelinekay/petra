// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/callback_table.hpp"
#include <iostream>
#include "petra/string_literal.hpp"

int main() {
  using namespace petra::literals;

  auto table = petra::make_callback_table(
      // std::make_tuple("square"_s, "circle"_s, "triangle"_s, "capsule"_s,
      // "hexagon"_s),
      std::make_tuple("square"_s, "circle"_s, "triangle"_s, "capsule"_s,
                      "hexagon"_s),
      std::make_tuple(
          []() {
            std::cout << "---" << std::endl;
            std::cout << "| |" << std::endl;
            std::cout << "---" << std::endl;
          },
          []() { std::cout << "o" << std::endl; },
          []() {
            std::cout << "/\\" << std::endl;
            std::cout << "--" << std::endl;
          },
          []() {
            std::cout << " ---" << std::endl;
            std::cout << "(   )" << std::endl;
            std::cout << " ---" << std::endl;
          },
          []() {
            std::cout << " _" << std::endl;
            std::cout << "/ \\" << std::endl;
            std::cout << "\\ /" << std::endl;
            std::cout << " -" << std::endl;
          }));

  std::string input;
  while (input != "exit") {
    std::cin >> input;
    table.trigger(input.c_str());
  }
  return 0;
}
