// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/indexed_bases.hpp"

#include "utilities.hpp"

#include <iostream>

#include <functional>

int main() {
  {
    int x0 = 0;
    int x1 = 0;
    int x2 = 0;
    auto zero = [&x0]() noexcept { ++x0; };
    auto one = [&x1]() noexcept { ++x1; };
    auto two = [&x2]() noexcept { ++x2; };

    auto table = petra::make_indexed_bases<void>(
        std::move(zero), std::move(one), std::move(two));
#ifdef __clang__
    static_assert(noexcept(table(std::declval<int>())));
#endif

    table(0);
    PETRA_ASSERT(x0 == 1);
    PETRA_ASSERT(x1 == 0);
    PETRA_ASSERT(x2 == 0);
    table(1);
    PETRA_ASSERT(x0 == 1);
    PETRA_ASSERT(x1 == 1);
    PETRA_ASSERT(x2 == 0);
    table(2);
    PETRA_ASSERT(x0 == 1);
    PETRA_ASSERT(x1 == 1);
    PETRA_ASSERT(x2 == 1);
  }
  {
    constexpr auto inc = [](int i) noexcept { return i + 1; };
    constexpr auto dec = [](int i) noexcept { return i - 1; };

    auto table = petra::make_indexed_bases<int>(std::move(inc), std::move(dec));

#ifdef __clang__
    static_assert(noexcept(table(std::declval<int>(), std::declval<int>())));
#endif

    PETRA_ASSERT(table(0, 0) == 1);
  }
}
