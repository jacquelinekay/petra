#include <iostream>
#include "dispatch/chd.hpp"

#include "dispatch/utilities.hpp"

struct printer {
  template<std::size_t N>
  void operator()(std::integral_constant<std::size_t, N>) const {
    std::cout << N << "\n";
  }
};

int main() {
  constexpr auto x = dispatch::make_chd(STRING_LITERAL("hello"),
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
}
