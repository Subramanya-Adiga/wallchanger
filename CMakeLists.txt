cmake_minimum_required(VERSION 3.16)
project(WallChanger LANGUAGES CXX)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(StanderdProjectSettings)
include(CompilerWarnings)
include(generate_windows_rc_file)
include(CTest)

add_executable(${PROJECT_NAME} main.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/wall_cache_library.cpp
                                       src/wall_background.cpp src/wall_app.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(${PROJECT_NAME} PRIVATE src/platform/platform_linux.cpp)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "-Wl,-v")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_WHAT_YOU_USE TRUE)

  find_package(PkgConfig REQUIRED)

  pkg_check_modules(DCONF REQUIRED dconf)
  target_include_directories(${PROJECT_NAME} PRIVATE ${DCONF_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${DCONF_LIBRARIES})

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(${PROJECT_NAME} PRIVATE src/platform/platform_win32.cpp)
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
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
  endif()
endif()
set_project_warnings(${PROJECT_NAME})
# Change To Find Boost Automatically on diffrent platforms
set(Boost_INCLUDE_DIR "/usr/local/include/boost-1_73")
add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_DEBUG ON)
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ARCHITECTURE -x64)
find_package(Boost 1.71 REQUIRED COMPONENTS container program_options)

# find_package(Qt5 REQUIRED COMPONENTS Core PATHS "~/.local/Qt/5.14.2/gcc_64"
# NO_DEFAULT_PATH)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++ -flto=thin)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
      LINK_FLAGS
      "-fuse-ld=lld -stdlib=libc++ -Wl,--verbose -Wl,--rpath=/usr/local/lib")
  target_link_libraries(${PROJECT_NAME} PRIVATE c++abi c++)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE -flto)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fuse-ld=gold")
endif()

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Boost::container spdlog::spdlog_header_only
                          range-v3::range-v3 Boost::program_options)
