#include <benchmark/benchmark.h>
#include <filesystem>
#include <wall_cache.hpp>

static void BM_SomeFunction(benchmark::State &state) {
  wallchanger::cache<std::string> cache;
  int i = 0;
  for (auto _ : state) {
    cache = wallchanger::cache<std::string>();
    for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
      cache.insert(std::forward<std::string>(x.path().filename().string()), 0);
      i++;
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);

static void BM_SomeFunction_MOVE(benchmark::State &state) {
  wallchanger::cache<std::string> cache;
  int i = 0;
  for (auto _ : state) {
    cache = wallchanger::cache<std::string>();
    for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
      cache.insert(x.path().filename().string(), 0);
      i++;
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction_MOVE);

// Run the benchmark
BENCHMARK_MAIN();