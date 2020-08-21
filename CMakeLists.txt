cmake_minimum_required(VERSION 3.18)
project(WallChanger LANGUAGES CXX C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
include(build_lua)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
  target_compile_options(project_options INTERFACE -march=znver2)
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID
                                                   STREQUAL "MSVC"))
  target_compile_options(project_options INTERFACE /arch:AVX2)
endif()

target_precompile_headers(
  project_options
  INTERFACE
  <vector>
  <string>
  <map>
  <utility>
  <string_view>
  <array>
  <algorithm>
  <memory>
  <range/v3/all.hpp>
  <fmt/format.h>
  <spdlog/spdlog.h>)

add_library(project_dependency INTERFACE)

# Main Executable Project
add_executable(changer main.cpp)
target_sources(
  changer PRIVATE src/wall_cache_library.cpp src/wall_background.cpp
                  src/wall_app.cpp src/wall_manager.cpp)
# set_target_properties(changer PROPERTIES CXX_EXTENSIONS OFF)
set_project_warnings(changer)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(changer PRIVATE src/platform/platform_linux.cpp)
  set_target_properties(changer PROPERTIES LINK_FLAGS_DEBUG "-Wl,-v")
  set_target_properties(changer PROPERTIES LINK_WHAT_YOU_USE TRUE)
  set(Boost_INCLUDE_DIR "/usr/local/include/boost-1_73")
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_ARCHITECTURE -x64)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(changer PUBLIC -v -g -O0)
  else()
    target_compile_options(changer PUBLIC -O2)
  endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(changer PRIVATE src/platform/platform_win32.cpp)
  if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                              "MSVC"))
    target_compile_options(changer PUBLIC /GL)
    set_target_properties(changer PROPERTIES LINK_FLAGS "/LTCG")
  endif()
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/spdlog")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/range-v3")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/fmtlib")
build_lua("${CMAKE_SOURCE_DIR}/vendor/lua")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/sol2")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/catch2")

add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_DEBUG OFF)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.71 REQUIRED COMPONENTS program_options)

# find_package(Qt5 REQUIRED COMPONENTS Core PATHS "~/.local/Qt/5.14.2/gcc_64"
# NO_DEFAULT_PATH)

function(set_link_flags TARGET)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND (NOT CMAKE_CXX_SIMULATE_ID
                                                 STREQUAL "MSVC"))
    target_compile_options(${TARGET} PRIVATE -stdlib=libc++ -flto=thin)
    set_target_properties(
      ${TARGET}
      PROPERTIES
        LINK_FLAGS
        "-fuse-ld=lld -stdlib=libc++ -Wl,--verbose -Wl,--rpath=/usr/local/lib")
    target_link_libraries(${TARGET} PRIVATE c++abi c++)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${TARGET} PRIVATE -flto)
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-fuse-ld=gold")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID
                                                    STREQUAL "MSVC")
    target_compile_options(${TARGET} PRIVATE -flto=thin)
  endif()
endfunction()

target_link_libraries(
  project_dependency
  INTERFACE spdlog::spdlog_header_only range-v3::range-v3
            Boost::program_options fmt::fmt-header-only lua::lua_static
            sol2::sol2)
set_link_flags(changer)
target_link_libraries(changer PRIVATE project_options project_dependency)

if(BUILD_TESTING)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/catch2/contrib")
  include(Catch)
  add_subdirectory("${CMAKE_SOURCE_DIR}/testing")
  add_subdirectory("${CMAKE_SOURCE_DIR}/benchmarking")

endif()
