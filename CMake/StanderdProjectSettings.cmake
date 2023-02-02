include(CTest)
include(CompilerWarnings)
include(generate_windows_rc_file)
include(ccache)
include(TargetArch)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# macro to build parts of project as release
macro(CHECK_IF_RELEASE)
  if(NOT (CMAKE_BUILD_TYPE MATCHES Release))
    get_filename_component(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REGEX REPLACE " " "_" THIS_DIR ${THIS_DIR})
    if(DEFINED RELEASE_${THIS_DIR})
      message(
        STATUS "Note: Targets in directory ${THIS_DIR} will be built Release")
      set(CMAKE_BUILD_TYPE Release)
    endif()
  endif()
endmacro()

option(ENABLE_IPO
       "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)"
       OFF)

if(ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output)
  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(SEND_ERROR "IPO is not supported: ${output}")
  endif()
endif()

function(use_libcxx TARGET ENABLE)
  if((CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND (NOT CMAKE_CXX_SIMULATE_ID
                                                  STREQUAL "MSVC")) AND (ENABLE
                                                                        ))
    target_compile_options(${TARGET} INTERFACE -stdlib=libc++)
    target_link_options(${TARGET} INTERFACE -stdlib=libc++
                        -Wl,--rpath=/usr/local/lib)
    message(STATUS "Using LIBC++")
  endif()
endfunction()
