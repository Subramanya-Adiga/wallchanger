project(testing CXX C)

add_executable(testing_cache testing_cache.cpp)
set_standerd_flag(testing_cache)
set_link_flags(testing_cache)
target_precompile_headers(testing_cache PRIVATE ../pch.h)
set_target_properties(testing_cache PROPERTIES LINK_FLAGS_DEBUG "-Wl,-v")
set_target_properties(testing_cache PROPERTIES LINK_WHAT_YOU_USE TRUE)
catch_discover_tests(testing_cache)

target_link_libraries(testing_cache PRIVATE changer Catch2::Catch2)
