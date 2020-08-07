project(testing CXX C)

add_library(test_project_options INTERFACE)

target_link_libraries(
  test_project_options INTERFACE project_options project_dependency
                                 Catch2::Catch2)

add_executable(test_main testing_cache.cpp)
set_link_flags(test_main)

# test discovery
catch_discover_tests(test_main)

target_link_libraries(test_main PRIVATE test_project_options)
