cmake_minimum_required(VERSION 3.30)

project(
  Wallchanger
  VERSION 1.0.0.0
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
include(FetchContent)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)

if(MINGW OR MSVC)
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
  <functional>
  <random>
  <nlohmann/json.hpp>)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG "v1.17.0")
set(SPDLOG_USE_STD_FORMAT ON)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG "v3.12.0")

FetchContent_Declare(
  lua
  GIT_REPOSITORY https://github.com/marovira/lua.git
  GIT_TAG "5.4.8")

FetchContent_Declare(
  sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG "v3.5.0")

FetchContent_MakeAvailable(spdlog nlohmann_json sol2 lua)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(asio REQUIRED)

target_link_libraries(
  project_dependency
  INTERFACE spdlog::spdlog_header_only sol2 lua::lua Boost::program_options
            nlohmann_json::nlohmann_json asio::asio)

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

add_custom_target(
  copy_compile_commands ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
  DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
  COMMENT "Copying compile_commands.json to source directory")

if(BUILD_TESTING)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG "v3.13.0")

  FetchContent_MakeAvailable(catch2)

  add_subdirectory("${CMAKE_SOURCE_DIR}/tests")
  add_subdirectory("${CMAKE_SOURCE_DIR}/benchmarking")

endif()
