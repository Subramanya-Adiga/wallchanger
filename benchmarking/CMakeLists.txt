project(benchmarking CXX C)

add_executable(bench_cache bench_cache.cpp)
target_precompile_headers(bench_cache INTERFACE <catch2/catch_all.hpp>)
# catch_discover_tests(test_cache)
target_link_libraries(
  bench_cache PRIVATE change_lib project_options project_dependency
                      Catch2::Catch2 Catch2::Catch2WithMain)

add_executable(bench_cache_google_bench bench_cache_google.cpp)
find_package(benchmark REQUIRED)
target_link_libraries(
  bench_cache_google_bench
  PRIVATE change_lib project_options project_dependency benchmark::benchmark
          benchmark::benchmark_main)
