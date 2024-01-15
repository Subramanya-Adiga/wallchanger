cmake_minimum_required(VERSION 3.18)

project(
  Wallchanger
  VERSION 1.0.0.0
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
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
  target_compile_definitions(project_options INTERFACE _CRT_SECURE_NO_WARNINGS
                                                       NOMINMAX UNICODE)
endif()

set_project_warnings(project_options)

add_library(project_dependency INTERFACE)
target_precompile_headers(
  project_dependency
  INTERFACE
  <vector>
  <string>
  <map>
  <unordered_map>
  <utility>
  <string_view>
  <array>
  <algorithm>
  <memory>
  <filesystem>
  <iostream>
  <tuple>
  <variant>
  <range/v3/all.hpp>
  <fmt/format.h>
  <nlohmann/json.hpp>
  "src/config.h")

find_package(range-v3 REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(sol2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
target_link_libraries(
  project_dependency
  INTERFACE range-v3::range-v3
            fmt::fmt-header-only
            sol2
            spdlog::spdlog_header_only
            Boost::program_options
            nlohmann_json::nlohmann_json
            asio::asio)

message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKEDIR CMake)
else()
  set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR
    ${DEF_INSTALL_CMAKEDIR}
    CACHE PATH "Installation directory for CMake files")

foreach(p LIB BIN INCLUDE CMAKE)
  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path)
  message(STATUS "Installing ${p} components to ${_path}")
  unset(_path)
endforeach()

# change_lib
add_subdirectory("${CMAKE_SOURCE_DIR}/src/change_lib")
# change_service
add_subdirectory("${CMAKE_SOURCE_DIR}/src/change_service")
# changer
add_subdirectory("${CMAKE_SOURCE_DIR}/src/change_app")

use_libcxx(project_options FALSE)

if(BUILD_TESTING)
  find_package(Catch2 REQUIRED)
  find_package(benchmark REQUIRED)
  include(Catch)
  add_subdirectory("${CMAKE_SOURCE_DIR}/tests")
  add_subdirectory("${CMAKE_SOURCE_DIR}/benchmarking")

endif()
