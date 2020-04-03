include(CheckIPOSupported)
include(CTest)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# macro to build parts of project as release
macro(CHECK_IF_RELEASE)
  if(NOT (CMAKE_BUILD_TYPE MATCHES Release))
    get_filename_component(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REGEX
           REPLACE " "
                   "_"
                   THIS_DIR
                   ${THIS_DIR})
    if(DEFINED RELEASE_${THIS_DIR})
      message(
        STATUS "Note: Targets in directory ${THIS_DIR} will be built Release")
      set(CMAKE_BUILD_TYPE Release)
    endif()
  endif()
endmacro()

check_ipo_supported(RESULT supported_ipo OUTPUT comp_name)
if(supported_ipo)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${comp_name}")
endif()

#Run Clang-tidy
function(static_analize_clang_tidy target_name)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
        set_target_properties(${target_name} PROPERTIES CXX_CLANG_TIDY ${CLANGTIDY})
    else()
        message(SEND_ERROR "clang-tidy executable not found")
    endif()
endfunction()

function(static_analize_cpp_check target_name)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        set_target_properties(${target_name} PROPERTIES CXX_CPPCHECK ${CPPCHECK})
    else()
        message(SEND_ERROR "cppcheck executable not found")
    endif()
endfunction()
