#include "../pch.h"
#include "../src/wall_cache.h"
#define CATCH_CONFIG_MAIN
#include "../src/wall_cache_library.cpp"
#include <catch2/catch.hpp>

TEST_CASE("Library Construction", "[cahce lib construct]") {
  using namespace std::literals;
  // test cache construction
  wall_changer::wall_cache_lib::cache_lib_type cache(0);
  cache.insert(0, "hello"s);

  wall_changer::wall_cache_lib lib1;
  lib1.insert("cache1", cache);
  REQUIRE(lib1.cache_count() == 1);

  auto lib2(lib1);
  REQUIRE(lib2.cache_count() == 1);
  REQUIRE(lib2 == lib1);

  auto lib3(std::move(lib1));
  REQUIRE(lib3 != lib1);
  REQUIRE(lib1.empty() == true);
}