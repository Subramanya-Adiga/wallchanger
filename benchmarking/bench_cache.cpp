#include <filesystem>
#include <wall_cache.h>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

TEST_CASE("Cache") {

  BENCHMARK_ADVANCED("CACHE INSERT")(Catch::Benchmark::Chronometer meter) {
    wallchanger::cache<int, std::string> cache;
    meter.measure([&] {
      cache = wallchanger::cache<int, std::string>();
      int i = 0;
      for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
        cache.insert(i, x.path().filename().string());
        i++;
      }
    });
  };

  BENCHMARK_ADVANCED("CACHE INSERT Forward")
  (Catch::Benchmark::Chronometer meter) {
    wallchanger::cache<int, std::string> cache;
    meter.measure([&] {
      cache = wallchanger::cache<int, std::string>();
      int i = 0;
      for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
        cache.insert(i,
                     std::forward<std::string>(x.path().filename().string()));
        i++;
      }
    });
  };
}