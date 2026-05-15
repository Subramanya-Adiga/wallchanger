# changer
add_executable(changer main.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(changer PRIVATE /GL)
  target_link_options(changer PRIVATE /LTCG)
endif()

target_sources(changer PRIVATE wall_app.cpp wall_app.hpp wall_cmds.hpp
                               wall_manager.cpp wall_manager.hpp)

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG "boost-1.91.0")

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_MakeAvailable(Boost)

target_link_libraries(changer PRIVATE change_lib Boost::program_options)
