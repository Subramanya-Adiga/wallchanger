project(benchmarking CXX C)

add_executable(bench_cache bench_cache.cpp)
set_link_flags(bench_cache)
target_precompile_headers(bench_cache INTERFACE <catch2/catch.hpp>)
# catch_discover_tests(test_cache)
target_link_libraries(bench_cache PRIVATE project_options project_dependency
                                          Catch2::Catch2)

add_executable(bench_cache_google_bench bench_cache_google.cpp)
set_link_flags(bench_cache_google_bench)
find_package(benchmark REQUIRED)
target_link_libraries(
  bench_cache_google_bench
  PRIVATE project_options project_dependency benchmark::benchmark
          benchmark::benchmark_main)
