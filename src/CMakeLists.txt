# change_service
add_executable(change_service service_main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(change_service PRIVATE /GL)
  target_link_options(change_service PRIVATE /LTCG)
endif()

target_sources(
  change_service
  PRIVATE service_main.cpp
          wall_cache_library.cpp
          wall_cache_library.h
          wall_manager.cpp
          wall_manager.h
          wall_cache.h
          json_helper.h
          net/net_common.h
          net/connect_queue.h
          net/connection.h
          net/client_interface.h
          net/server_interface.h)

target_link_libraries(change_service PRIVATE project_options project_dependency)

if(WIN32)
  target_sources(
    change_service
    PRIVATE change_service.h platform/win32_service.cpp
            platform/win32_service.h platform/windows_helper.h
            platform/windows_helper.cpp)
endif()

# changer
add_executable(changer main.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_SIMULATE_ID STREQUAL
                                            "MSVC"))
  target_compile_options(changer PRIVATE /GL)
  target_link_options(changer PRIVATE /LTCG)
endif()

target_sources(changer PRIVATE wall_app.cpp wall_app.h wall_background.cpp
                               wall_background.h)

if(WIN32)
  target_sources(
    changer PRIVATE platform/windows_helper.cpp platform/windows_helper.h
                    platform/platform_win32.cpp platform/platform_win32.h)
endif()

target_link_libraries(changer PRIVATE project_options project_dependency)
