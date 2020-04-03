cmake_minimum_required(VERSION 3.16)
project(WallChanger LANGUAGES CXX)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
include(CompilerWarnings)
include(generate_windows_rc_file)
include(CTest)

add_executable(${PROJECT_NAME} main.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
 # target_sources(${PROJECT_NAME} PRIVATE src/platform/plat_gnome_bg.cpp)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "-Wl,-v")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_WHAT_YOU_USE TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(${PROJECT_NAME} PRIVATE src/platform_windows.cpp)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/spdlog")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/range-v3")
if(BUILD_TESTING)
endif()

if(cxx_std_20 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
else()
  target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS ON)
endif()

if(NOT MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -v -g -O0
                                                   -fno-omit-frame-pointer)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O2 -flto=thin)
  endif()
endif()
set_project_warnings(${PROJECT_NAME})

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.71 REQUIRED COMPONENTS serialization container)

find_package(Qt5 REQUIRED COMPONENTS Core)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fuse-ld=lld")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fuse-ld=gold")
endif()

target_link_libraries(${PROJECT_NAME}
                      PRIVATE Boost::serialization Boost::container Qt5::Core 
                              spdlog::spdlog range-v3::range-v3)
