add_library(change_lib STATIC)

target_sources(
  change_lib
  PRIVATE wall_cache_library.cpp wall_background.cpp path_table.cpp
          wall_error.cpp json_helper.h
  PUBLIC FILE_SET
         headers
         TYPE
         HEADERS
         FILES
         wall_error.hpp
         message_type.h
         path_table.h
         wall_cache_library.h
         wall_cache.h
         wall_background.h
         net/net_common.h
         net/connect_queue.h
         net/connection.h
         net/client_interface.h
         net/server_interface.h)

if(WIN32)
  target_sources(
    change_lib
    PRIVATE platform/platform_win32.cpp platform/win32_service.cpp
            platform/windows_helper.cpp
    PUBLIC FILE_SET
           headers
           TYPE
           HEADERS
           FILES
           platform/platform_win32.h
           platform/win32_service.h
           platform/windows_helper.h)
  set_target_properties(
    change_lib PROPERTIES PUBLIC_HEADER platform/platform_win32.h
                          platform/win32_service.h platform/windows_helper.h)
endif()

if(LINUX)
  target_sources(
    change_lib
    PRIVATE platform/platform_linux.cpp
    PUBLIC FILE_SET headers TYPE HEADERS FILES platform/platform_linux.h)
  set_target_properties(change_lib PROPERTIES PUBLIC_HEADER
                                              platform/platform_linux.h)
endif()

target_link_libraries(change_lib PUBLIC project_options project_dependency)
