add_library(change_lib STATIC)

target_sources(
  change_lib
  PRIVATE wall_cache_library.cpp wall_background.cpp path_table.cpp
          wall_error.cpp wall_manager.cpp json_helper.hpp
  PUBLIC FILE_SET
         headers
         TYPE
         HEADERS
         FILES
         defines.hpp
         helpers.cpp
         helpers.hpp
         wall_manager.hpp
         crc32.hpp
         wall_error.hpp
         message_type.hpp
         path_table.hpp
         wall_cache_library.hpp
         wall_cache.hpp
         wall_background.hpp
         known-folders.cpp
         known-folders.hpp
         net/net_common.hpp
         net/connect_queue.hpp
         net/connection.hpp
         net/client_interface.hpp
         net/server_interface.hpp)

if(WIN32)
  target_sources(
    change_lib
    PRIVATE platform/win32/bg_handler.cpp platform/win32/service.cpp
            platform/win32/helper.cpp platform/win32/common.cpp
            platform/win32/win32_known_folders.cpp
    PUBLIC FILE_SET
           headers
           TYPE
           HEADERS
           FILES
           platform/win32/common.hpp
           platform/win32/win32_known_folders.hpp
           platform/win32/bg_handler.hpp
           platform/win32/service.hpp
           platform/win32/helper.hpp)
  set_target_properties(
    change_lib PROPERTIES PUBLIC_HEADER platform/win32/bg_handler.hpp
                          platform/win32/service.hpp platform/win32/helper.hpp)
endif()

if(LINUX)
  target_sources(
    change_lib
    PRIVATE platform/linux/bg_handler.cpp platform/linux/common.cpp
            platform/linux/linux_known_folders.cpp
    PUBLIC FILE_SET
           headers
           TYPE
           HEADERS
           FILES
           platform/linux/bg_handler.hpp
           platform/linux/common.hpp
           platform/linux/linux_known_folders.hpp)
  set_target_properties(change_lib PROPERTIES PUBLIC_HEADER
                                              platform/linux/bg_handler.hpp)
endif()

target_link_libraries(change_lib PUBLIC project_options project_dependency)
