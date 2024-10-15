add_library(change_lib STATIC)

target_sources(
  change_lib
  PRIVATE wall_cache_library.cpp wall_background.cpp path_table.cpp
          wall_error.cpp json_helper.hpp
  PUBLIC FILE_SET
         headers
         TYPE
         HEADERS
         FILES
         wall_error.hpp
         message_type.hpp
         path_table.hpp
         wall_cache_library.hpp
         wall_cache.hpp
         wall_background.hpp
         net/net_common.hpp
         net/connect_queue.hpp
         net/connection.hpp
         net/client_interface.hpp
         net/server_interface.hpp)

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
           platform/platform_win32.hpp
           platform/win32_service.hpp
           platform/windows_helper.hpp)
  set_target_properties(
    change_lib
    PROPERTIES PUBLIC_HEADER platform/platform_win32.hpp
               platform/win32_service.hpp platform/windows_helper.hpp)
endif()

if(LINUX)
  target_sources(
    change_lib
    PRIVATE platform/platform_linux.cpp
    PUBLIC FILE_SET headers TYPE HEADERS FILES platform/platform_linux.hpp)
  set_target_properties(change_lib PROPERTIES PUBLIC_HEADER
                                              platform/platform_linux.hpp)
endif()

target_link_libraries(change_lib PUBLIC project_options project_dependency)
