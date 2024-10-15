# change_service
add_executable(change_service service_main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(change_service PRIVATE /GL)
  target_link_options(change_service PRIVATE /LTCG)
endif()

target_sources(change_service PRIVATE service_main.cpp change_server.hpp
                                      change_state.cpp change_state.hpp)

target_link_libraries(change_service PRIVATE change_lib)

if(WIN32)
  target_sources(change_service PRIVATE change_service.hpp)
endif()
