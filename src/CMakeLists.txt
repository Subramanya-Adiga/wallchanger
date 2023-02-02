# Main Executable Project

add_executable(changer main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(changer PRIVATE /GL)
  target_link_options(changer PRIVATE /LTCG)
endif()

target_sources(
  changer
  PRIVATE main.cpp
          wall_cache_library.cpp
          wall_cache_library.h
          wall_background.cpp
          wall_background.h
          wall_app.cpp
          wall_app.h
          wall_manager.cpp
          wall_manager.h
          wall_cache.h)

target_link_libraries(changer PRIVATE project_options project_dependency)
