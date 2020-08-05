cmake_minimum_required(VERSION 3.16)
project(WallChanger LANGUAGES CXX C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
include(build_lua)

add_library(changer STATIC)
target_sources(changer PRIVATE src/wall_cache_library.cpp
                                       src/wall_background.cpp src/wall_app.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(changer PRIVATE src/platform/platform_linux.cpp)
  set_target_properties(changer PROPERTIES LINK_FLAGS_DEBUG "-Wl,-v")
  set_target_properties(changer PROPERTIES LINK_WHAT_YOU_USE TRUE)
  set(Boost_INCLUDE_DIR "/usr/local/include/boost-1_73")
  set(Boost_USE_STATIC_LIBS ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(Boost_INCLUDE_DIR "C:/boost/include/boost-1_73")
  target_sources(changer PRIVATE src/platform/platform_win32.cpp)
endif()

target_precompile_headers(changer PRIVATE pch.h)

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/spdlog")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/range-v3")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/fmtlib")
build_lua("${CMAKE_SOURCE_DIR}/vendor/lua" changer)
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/sol2")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/catch2")

add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_DEBUG OFF)
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_ARCHITECTURE -x64)

find_package(Boost 1.71 REQUIRED COMPONENTS container program_options)

if(NOT MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(changer INTERFACE -v -g -O0
                                                     -fno-omit-frame-pointer)
  else()
    target_compile_options(changer INTERFACE -O2)
  endif()
endif()
set_project_warnings(changer)

function(set_standerd_flag TARGET)
  if(cxx_std_20 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(${TARGET} PRIVATE cxx_std_20)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
  else()
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS ON)
  endif()
endfunction()

set_standerd_flag(changer)
# find_package(Qt5 REQUIRED COMPONENTS Core PATHS "~/.local/Qt/5.14.2/gcc_64"
# NO_DEFAULT_PATH)

function(set_link_flags TARGET)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT "Clang-CL")
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
  endif()
endfunction()

set_link_flags(changer)

target_link_libraries(
  changer
  PUBLIC Boost::container
         spdlog::spdlog_header_only
         range-v3::range-v3
         Boost::program_options
         fmt::fmt-header-only
         lua::lua_static
         sol2::sol2)

add_executable(changer_app main.cpp)
set_standerd_flag(changer_app)
set_link_flags(changer_app)
target_precompile_headers(changer_app PRIVATE pch.h)
target_link_libraries(changer_app PRIVATE changer)

if(BUILD_TESTING)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/catch2/contrib")
  include(Catch)
  add_subdirectory("${CMAKE_SOURCE_DIR}/testing")
endif()
