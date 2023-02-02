#include "../src/wall_cache.h"
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

TEST_CASE("Cache Construction", "[cache construct]") {
  using namespace std::literals;
  wallchanger::cache<int, std::string> cache(0);
  REQUIRE(cache.capacity() == std::numeric_limits<char>::max());

  wallchanger::cache<int, std::string> cache2(5);
  cache2.insert(0, "hello"s);
  REQUIRE(cache2.get(0) == "hello"s);
  REQUIRE(cache2.capacity() == 5);

  wallchanger::cache<int, std::string> cache3(cache2);
  REQUIRE(cache3.get(0) == "hello"s);
  REQUIRE(cache3 == cache2);

  wallchanger::cache<int, std::string> cache4(std::move(cache2));
  REQUIRE(cache4.capacity() == 5);
  REQUIRE(cache4.get(0) == "hello"s);
  REQUIRE(cache2 != cache4);
  REQUIRE(cache2.empty() == true);
}

TEST_CASE("Cache Functionality Tests", "[cache function]") {
  using namespace std::literals;
  wallchanger::cache<int, std::string> cache(5);
  REQUIRE(cache.capacity() == 5);

  SECTION("Insection") {
    cache.insert(0, "hello world"s);
    REQUIRE(cache.get(0) == "hello world"s);
    REQUIRE_FALSE(cache.empty());
    REQUIRE(cache.exists(0));
  }
  SECTION("Replace") {
    cache.insert(0, "hello world"s);
    cache.replace(0, "hello world!"s);
    CHECK(cache.get(0) == "hello world!");
    REQUIRE(cache.empty() == false);
    REQUIRE(cache.modified());
  }
  SECTION("Erase") {
    cache.insert(0, "hello world"s);
    REQUIRE_FALSE(cache.empty());
    cache.erase(0);
    REQUIRE(cache.empty() == true);
    REQUIRE(cache.modified() == true);
  }
  SECTION("State Change") {
    cache.insert(0, "hello world"s);
    cache.insert(1, "hello world"s);
    cache.insert(2, "hello world"s);

    cache.set_state(0, wallchanger::cache_state_e::unused);
    cache.set_state(1, wallchanger::cache_state_e::favorate);
    cache.set_state(2, wallchanger::cache_state_e::used);

    REQUIRE(cache.get_state(0) == wallchanger::cache_state_e::unused);
    REQUIRE(cache.get_state(1) == wallchanger::cache_state_e::favorate);
    REQUIRE(cache.get_state(2) == wallchanger::cache_state_e::used);
  }
}
