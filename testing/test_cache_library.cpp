#include "../pch.h"
#include "../src/wall_cache.h"
#define CATCH_CONFIG_MAIN
#include "../src/wall_cache_library.cpp"
#include <catch2/catch.hpp>

TEST_CASE("Library Construction", "[cahce lib construct]") {
  using namespace std::literals;
  // test cache construction
  wallchanger::cache_lib::cache_lib_type cache(0);
  cache.insert(0, "hello"s);

  wallchanger::cache_lib lib1;
  lib1.insert("cache1", cache);
  REQUIRE(lib1.cache_count() == 1);

  auto lib2(lib1);
  REQUIRE(lib2.cache_count() == 1);
  REQUIRE(lib2 == lib1);

  auto lib3(std::move(lib1));
  REQUIRE(lib3 != lib1);
  REQUIRE(lib1.empty() == true);
}

TEST_CASE("Library Functionality", "[cache lib function]") {
  using namespace std::literals;
  wallchanger::cache_lib::cache_lib_type cache(0);
  cache.insert(0, "hello"s);
  cache.insert(1, "world"s);
  wallchanger::cache_lib::cache_lib_type cache2(0);

  SECTION("insection") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    REQUIRE(lib.modified() == true);
    REQUIRE(lib.empty() == false);
  }
  SECTION("exists") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    REQUIRE(lib.exists("cache1") == true);
    REQUIRE(lib.exists("cache2") == true);
  }
  SECTION("get cache") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    REQUIRE(lib.get_cache("cache2").empty() == true);
    REQUIRE(lib["cache1"].empty() == false);
  }
  SECTION("erase") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    lib.remove("cache1");
    REQUIRE(lib.exists("cache1") == false);
  }
}