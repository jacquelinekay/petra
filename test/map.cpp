// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#include "petra/map.hpp"
#include "utilities.hpp"

using namespace petra::literals;

int main() {
  auto test_map = petra::make_map(
      std::make_pair("asdf"_s, 0), std::make_pair("qwerty"_s, 0.0),
      std::make_pair("quux"_s, 0.0f), std::make_pair("foo"_s, false),
      std::make_pair("bar"_s, std::string("hello")));

  PETRA_ASSERT(test_map.at<int>("asdf").valid());
  PETRA_ASSERT(*test_map.at<int>("asdf").value() == 0);
  PETRA_ASSERT(*test_map.at<double>("qwerty").value() == 0.0);
  PETRA_ASSERT(*test_map.at<float>("quux").value() == 0.0f);
  PETRA_ASSERT(*test_map.at<bool>("foo").value() == false);
  PETRA_ASSERT(*test_map.at<std::string>("bar").value() == "hello");

  PETRA_ASSERT(test_map.insert("asdf", 1) == petra::MapAccessStatus::success);
  PETRA_ASSERT(test_map.insert("qwerty", 2.0)
               == petra::MapAccessStatus::success);
  PETRA_ASSERT(test_map.insert("quux", 3.0f)
               == petra::MapAccessStatus::success);
  PETRA_ASSERT(test_map.insert("foo", true) == petra::MapAccessStatus::success);
  PETRA_ASSERT(test_map.insert("bar", std::string("world"))
               == petra::MapAccessStatus::success);

  PETRA_ASSERT(*test_map.at<int>("asdf").value() == 1);
  PETRA_ASSERT(*test_map.at<double>("qwerty").value() == 2.0);
  PETRA_ASSERT(*test_map.at<float>("quux").value() == 3.0f);
  PETRA_ASSERT(*test_map.at<bool>("foo").value() == true);
  PETRA_ASSERT(*test_map.at<std::string>("bar").value() == "world");

  PETRA_ASSERT(test_map.at<std::string>("asdf").value() == nullptr);
  PETRA_ASSERT(*test_map.at<std::string>("asdf").error()
               == petra::MapAccessStatus::key_type_mismatch);

  PETRA_ASSERT(test_map.at<std::vector<int>>("qwerty").value() == nullptr);
  PETRA_ASSERT(*test_map.at<std::vector<int>>("qwerty").error()
               == petra::MapAccessStatus::key_type_mismatch);

  PETRA_ASSERT(test_map.at<void*>("quux").value() == nullptr);
  PETRA_ASSERT(*test_map.at<void*>("quux").error()
               == petra::MapAccessStatus::key_type_mismatch);

  PETRA_ASSERT(test_map.at<float>("foo").value() == nullptr);
  PETRA_ASSERT(*test_map.at<float>("foo").error()
               == petra::MapAccessStatus::key_type_mismatch);

  PETRA_ASSERT(test_map.at<void (*)(int)>("bar").value() == nullptr);
  PETRA_ASSERT(*test_map.at<void (*)(int)>("bar").error()
               == petra::MapAccessStatus::key_type_mismatch);

  PETRA_ASSERT(test_map.insert("asdf", std::string("hello"))
               == petra::MapAccessStatus::key_type_mismatch);
  PETRA_ASSERT(test_map.insert("qwerty", std::array<int, 2>{{1, 2}})
               == petra::MapAccessStatus::key_type_mismatch);
  PETRA_ASSERT(test_map.insert("quux", []() { std::cout << "hi\n"; })
               == petra::MapAccessStatus::key_type_mismatch);
  PETRA_ASSERT(test_map.insert("foo", 3)
               == petra::MapAccessStatus::key_type_mismatch);
  PETRA_ASSERT(test_map.insert("bar", 0.4f)
               == petra::MapAccessStatus::key_type_mismatch);

  constexpr std::size_t map_size = decltype(test_map)::size;
  std::array<std::size_t, map_size> results = {{0}};

  for (std::size_t i = 0; i < map_size; ++i) {
    test_map.key_at(i).then([&test_map, &results, &i](auto&& key) {
      test_map.visit(key, [&results, &i](const auto& x) {
        std::cout << x << std::endl;
        ++results[i];
      });
    });
  }

  for (std::size_t i = 0; i < map_size; ++i) { PETRA_ASSERT(results[i] == 1); }
  return 0;
}
