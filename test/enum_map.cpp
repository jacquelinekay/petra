// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/enum_map.hpp"
#include "utilities.hpp"

#include <unordered_map>

enum struct Color { Red, Green, Blue };

template<Color c>
using color_constant = std::integral_constant<Color, c>;

struct test {
  template<Color A, Color B>
  void operator()(color_constant<A>&&, color_constant<B>&&) noexcept {
    PETRA_ASSERT(A == B);
    PETRA_ASSERT(++results[A] == 1);
  }

  template<Color A, typename T>
  void operator()(color_constant<A>&&, T) noexcept {
    PETRA_ASSERT(false);
  }

  template<typename T>
  void operator()(petra::InvalidInputError&&, T&&) noexcept {
    PETRA_ASSERT(petra::utilities::is_error_type<T>());
    for (const auto& result : results) { PETRA_ASSERT(result.second == 1); }
  }

  std::unordered_map<Color, std::size_t> results;
};

int main() {
  {
    auto enum_table =
        petra::make_enum_map<Color, Color::Red, Color::Green, Color::Blue>(
            test{});

#ifdef __clang__
    static_assert(noexcept(
        enum_table(std::declval<Color>(), color_constant<Color::Red>{})));
    static_assert(noexcept(
        enum_table(std::declval<Color>(), color_constant<Color::Green>{})));
    static_assert(noexcept(
        enum_table(std::declval<Color>(), color_constant<Color::Blue>{})));
#endif

    enum_table(Color::Red, color_constant<Color::Red>{});
    enum_table(Color::Green, color_constant<Color::Green>{});
    enum_table(Color::Blue, color_constant<Color::Blue>{});

    std::underlying_type_t<Color> x = 4;
    enum_table(static_cast<Color>(x), petra::InvalidInputError{});
  }

  {
    auto enum_table =
        petra::make_enum_map<Color, Color::Red, Color::Green, Color::Blue>(
            [](auto&&) { throw std::runtime_error("Catch this!"); });

    static_assert(!noexcept(enum_table(std::declval<Color>())));
  }
}
