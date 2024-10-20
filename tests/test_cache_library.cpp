#include <wall_cache.hpp>
#include <wall_cache_library.hpp>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

TEST_CASE("Library Construction", "[cahce lib construct]") {
  using namespace std::literals;
  // test cache construction
  wallchanger::cache_lib::cache_lib_type cache;
  cache.insert("hello"s, 0);

  wallchanger::cache_lib lib1;
  lib1.insert("cache1", cache);
  REQUIRE(lib1.cache_count() == 1);

  auto lib2(lib1);
  REQUIRE(lib2.cache_count() == 1);
  REQUIRE(lib2 == lib1);

  auto lib3(std::move(lib1));
  REQUIRE(lib3 != lib1);
  REQUIRE(lib1.is_empty() == true);
}

TEST_CASE("Library Functionality", "[cache lib function]") {
  using namespace std::literals;
  wallchanger::cache_lib::cache_lib_type cache;
  cache.insert("hello"s, 0);
  cache.insert("world"s, 0);
  wallchanger::cache_lib::cache_lib_type cache2;

  SECTION("insection") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    REQUIRE(lib.modified() == true);
    REQUIRE(lib.is_empty() == false);
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
    REQUIRE(lib.get_cache("cache2").value().get().empty() == true);
    REQUIRE(lib.get_cache("cache1").value().get().empty() == false);
  }
  SECTION("erase") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    lib.remove("cache1");
    REQUIRE(lib.exists("cache1") == false);
  }
  SECTION("get current") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    lib.insert("cache2", cache2);
    auto lib55 = lib.get_cache("cache1");
    lib.change_active("cache1");
    REQUIRE(lib.get_current().value().get() == cache);
  }
  SECTION("merge") {
    wallchanger::cache_lib lib;
    lib.insert("cache1", cache);
    cache2.insert("merge"s, 0);
    cache2.insert("test"s, 0);
    cache2.insert("brave"s, 0);

    lib.insert("cache2", cache2);
    wallchanger::cache_lib::cache_lib_type new_cache(cache.size() +
                                                     cache2.size());
    std::ranges::merge(cache, cache2, new_cache.begin());
    lib.merge_cache("cache1", "cache2");
    auto merged = lib.get_cache("cache1").value().get();
    REQUIRE(new_cache == merged);
  }
}
