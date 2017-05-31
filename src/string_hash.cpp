#include <iostream>

#include "dispatch/chd.hpp"
#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"

using namespace dispatch::literals;

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }
};

int main() {
  constexpr auto x = dispatch::make_chd("hello"_s,
      "goodbye"_s,
      "dog"_s,
      "fish"_s,
      "cat"_s);
  using Hash = decltype(x);

  std::cout << "hash(hello) => " << Hash::hash("hello") << "\n";
  std::cout << "hash(goodbye) => " << Hash::hash("goodbye") << "\n";
  std::cout << "hash(cat) => " << Hash::hash("cat") << "\n";
  std::cout << "hash(dog) => " << Hash::hash("dog") << "\n";
  std::cout << "hash(fish) => " << Hash::hash("fish") << "\n";
}
