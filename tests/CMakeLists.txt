project(testing CXX C)

add_executable(test_cache testing_cache.cpp)
target_precompile_headers(test_cache INTERFACE <catch2/catch_all.hpp>)
catch_discover_tests(test_cache)
target_link_libraries(
  test_cache PRIVATE change_lib project_options project_dependency
                     Catch2::Catch2 Catch2::Catch2WithMain)

# Test CacheLibrary
add_executable(test_cache_library test_cache_library.cpp)
target_precompile_headers(test_cache_library REUSE_FROM test_cache)
catch_discover_tests(test_cache_library)
target_link_libraries(
  test_cache_library PRIVATE change_lib project_options project_dependency
                             Catch2::Catch2 Catch2::Catch2WithMain)
