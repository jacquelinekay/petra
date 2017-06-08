// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/enum_map.hpp"
#include "utilities.hpp"

enum struct Color { Red, Green, Blue, Error };

template<Color c>
using color_constant = std::integral_constant<Color, c>;

int main() {
  auto enum_table =
      petra::make_enum_map<Color, Color::Red, Color::Green, Color::Blue>(
          [](auto&& e_constant, auto&& test_constant) {
            using TestType = std::decay_t<decltype(test_constant)>;
            using EnumType = std::decay_t<decltype(e_constant)>;
            static_assert(std::is_same<typename EnumType::value_type, Color>{});
            static_assert(std::is_same<typename TestType::value_type, Color>{});
            PETRA_ASSERT(EnumType::value == TestType::value);
          });

  enum_table(Color::Red, color_constant<Color::Red>{});
  enum_table(Color::Green, color_constant<Color::Green>{});
  enum_table(Color::Blue, color_constant<Color::Blue>{});
}
