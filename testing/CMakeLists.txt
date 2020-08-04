project(testing CXX C)

add_executable(testing_cache testing_cache.cpp)
target_compile_features(testing_cache PRIVATE cxx_std_20)
set_target_properties(testing_cache PROPERTIES CXX_EXTENSIONS OFF)

catch_discover_tests(testing_cache)

target_link_libraries(
  testing_cache
  PRIVATE Boost::container
          spdlog::spdlog_header_only
          range-v3::range-v3
          Boost::program_options
          fmt::fmt-header-only
          lua::lua_static
          sol2::sol2
          Catch2::Catch2)
