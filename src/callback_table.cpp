#include <iostream>
#include "dispatch/callback_table.hpp"
#include "dispatch/string_literal.hpp"

int main() {
  using namespace dispatch::literals;

  auto table = dispatch::make_callback_table(
      // std::make_tuple("square"_s, "circle"_s, "triangle"_s, "capsule"_s, "hexagon"_s),
      std::make_tuple(
          "square"_s, "circle"_s,
          "triangle"_s, "capsule"_s,
          "hexagon"_s),
      std::make_tuple(
          []() {
            std::cout << "---" << std::endl;
            std::cout << "| |" << std::endl;
            std::cout << "---" << std::endl;
          },
          []() {
            std::cout << "o" << std::endl;
          },
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
          }
      )
  );

  while (1) {
    std::string input;
    std::cin >> input;
    table.trigger(input.c_str());
  }
}
