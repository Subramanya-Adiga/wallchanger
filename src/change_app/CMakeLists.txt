# changer
add_executable(changer main.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(changer PRIVATE /GL)
  target_link_options(changer PRIVATE /LTCG)
endif()

target_sources(changer PRIVATE wall_app.cpp wall_app.hpp change_client.hpp)

target_link_libraries(changer PRIVATE change_lib)
