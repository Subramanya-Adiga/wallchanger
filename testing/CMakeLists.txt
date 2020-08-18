project(testing CXX C)

add_executable(test_cache testing_cache.cpp)
set_link_flags(test_cache)
target_precompile_headers(test_cache INTERFACE <catch2/catch.hpp>)
catch_discover_tests(test_cache)
target_link_libraries(test_cache PRIVATE project_options project_dependency
                                         Catch2::Catch2)

# Test CacheLibrary
add_executable(test_cache_library test_cache_library.cpp)
set_link_flags(test_cache_library)
target_precompile_headers(test_cache_library REUSE_FROM test_cache)
catch_discover_tests(test_cache_library)
target_link_libraries(test_cache_library
                      PRIVATE project_options project_dependency Catch2::Catch2)
