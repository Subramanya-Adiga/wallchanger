#include <benchmark/benchmark.h>
#include <filesystem>
#include <wall_cache.h>


static void BM_SomeFunction(benchmark::State &state) {
  wallchanger::cache<int, std::string> cache;
  int i = 0;
  for (auto _ : state) {
    cache = wallchanger::cache<int, std::string>();
    for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
      cache.insert(i, std::forward<std::string>(x.path().filename().string()));
      i++;
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);

static void BM_SomeFunction_MOVE(benchmark::State &state) {
  wallchanger::cache<int, std::string> cache;
  int i = 0;
  for (auto _ : state) {
    cache = wallchanger::cache<int, std::string>();
    for (auto &&x : std::filesystem::directory_iterator("D:/Wallpaper")) {
      cache.insert(i, x.path().filename().string());
      i++;
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction_MOVE);

// Run the benchmark
BENCHMARK_MAIN();