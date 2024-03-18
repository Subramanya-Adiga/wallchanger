#include <wall_cache.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

TEST_CASE("Cache Construction", "[cache construct]") {
  using namespace std::literals;
  wallchanger::cache<std::string> cache;

  wallchanger::cache<std::string> cache2;
  cache2.insert("hello"s, 0);
  REQUIRE(cache2[0].cache_value == "hello"s);

  wallchanger::cache<std::string> cache3(cache2);
  REQUIRE(cache3[0].cache_value == "hello"s);
  REQUIRE(cache3 == cache2);

  wallchanger::cache<std::string> cache4(std::move(cache2));
  REQUIRE(cache4[0].cache_value == "hello"s);
  REQUIRE(cache2 != cache4);
  REQUIRE(cache2.empty() == true);
}

TEST_CASE("Cache Functionality Tests", "[cache function]") {
  using namespace std::literals;
  wallchanger::cache<std::string> cache;

  SECTION("Insection") {
    cache.insert("hello world"s, 0);
    REQUIRE(cache[0].cache_value == "hello world"s);
    REQUIRE_FALSE(cache.empty());
    REQUIRE(cache.contains("hello world"));
  }
}
