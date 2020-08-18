project(benchmarking CXX C)

add_executable(bench_cache bench_cache.cpp)
set_link_flags(bench_cache)
target_precompile_headers(bench_cache INTERFACE <catch2/catch.hpp>)
# catch_discover_tests(test_cache)
target_link_libraries(bench_cache PRIVATE project_options project_dependency
                                          Catch2::Catch2)
