#include <filesystem>
#include <wall_cache.hpp>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

TEST_CASE("Cache") {

  BENCHMARK_ADVANCED("CACHE INSERT")(Catch::Benchmark::Chronometer meter) {
    wallchanger::cache<std::string> cache;
    meter.measure([&] {
      cache = wallchanger::cache<std::string>();
      int i = 0;
      for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
        cache.insert(x.path().filename().string(), 0);
        i++;
      }
    });
  };

  BENCHMARK_ADVANCED("CACHE INSERT Forward")
  (Catch::Benchmark::Chronometer meter) {
    wallchanger::cache<std::string> cache;
    meter.measure([&] {
      cache = wallchanger::cache<std::string>();
      int i = 0;
      for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
        cache.insert(std::forward<std::string>(x.path().filename().string()),
                     0);
        i++;
      }
    });
  };
}