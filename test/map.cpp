#include "petra/map.hpp"
#include "utilities.hpp"

using namespace petra::literals;

int main() {
  auto test_map = petra::make_map(
    std::make_pair("asdf"_s, 0),
    std::make_pair("qwerty"_s, 0.0),
    std::make_pair("quux"_s, 0.0f),
    std::make_pair("foo"_s, false),
    std::make_pair("bar"_s, std::string("hello"))
  );
  PETRA_ASSERT(*test_map.at<int>("asdf") == 0);
  PETRA_ASSERT(*test_map.at<double>("qwerty") == 0.0);
  PETRA_ASSERT(*test_map.at<float>("quux") == 0.0f);
  PETRA_ASSERT(*test_map.at<bool>("foo") == false);
  PETRA_ASSERT(*test_map.at<std::string>("bar") == "hello");

  PETRA_ASSERT(test_map.insert("asdf", 1) != nullptr);
  PETRA_ASSERT(test_map.insert("qwerty", 2.0) != nullptr);
  PETRA_ASSERT(test_map.insert("quux", 3.0f) != nullptr);
  PETRA_ASSERT(test_map.insert("foo", true) != nullptr);
  PETRA_ASSERT(test_map.insert("bar", std::string("world")) != nullptr);

  PETRA_ASSERT(*test_map.at<int>("asdf") == 1);
  PETRA_ASSERT(*test_map.at<double>("qwerty") == 2.0);
  PETRA_ASSERT(*test_map.at<float>("quux") == 3.0f);
  PETRA_ASSERT(*test_map.at<bool>("foo") == true);
  PETRA_ASSERT(*test_map.at<std::string>("bar") == "world");

  PETRA_ASSERT(test_map.at<std::string>("asdf") == nullptr);
  PETRA_ASSERT(test_map.at<std::vector<int>>("qwerty") == nullptr);
  PETRA_ASSERT(test_map.at<void*>("quux") == nullptr);
  PETRA_ASSERT(test_map.at<float>("foo") == nullptr);
  PETRA_ASSERT(test_map.at<void(*)(int)>("bar") == nullptr);

  PETRA_ASSERT(test_map.insert("asdf", std::string("hello")) == nullptr);
  PETRA_ASSERT(test_map.insert("qwerty", std::array<int, 2>{{1, 2}}) == nullptr);
  PETRA_ASSERT(test_map.insert("quux", [](){ std::cout << "hi\n"; }) == nullptr);
  PETRA_ASSERT(test_map.insert("foo", 3) == nullptr);
  PETRA_ASSERT(test_map.insert("bar", 0.4f) == nullptr);

  constexpr std::size_t map_size = decltype(test_map)::size;
  std::array<std::size_t, map_size> results = {{0}};

  for (std::size_t i = 0; i < map_size; ++i) {
    test_map.visit(
        test_map.key_at(i),
        [&results, &i](const auto& x){
          std::cout << x << std::endl;
          ++results[i];
        });
  }

  for (std::size_t i = 0; i < map_size; ++i) {
    PETRA_ASSERT(results[i] == 1);
  }
  return 0;
}

