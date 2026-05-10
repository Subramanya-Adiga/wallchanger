project(benchmarking CXX C)

add_executable(bench_cache bench_cache.cpp)
target_precompile_headers(bench_cache INTERFACE <catch2/catch_all.hpp>)
target_link_libraries(
  bench_cache PRIVATE change_lib project_options project_dependency
                      Catch2::Catch2 Catch2::Catch2WithMain)
