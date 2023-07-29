include(windeployqt)

add_executable(changer_gui WIN32 gui_main.cpp thumblist_model.h
                                 thumblist_model.cpp
                                 thumblist_data.h thumblist_data.cpp)

set_target_properties(
  changer_gui
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

target_sources(changer_gui PRIVATE mainwindow.cpp mainwindow.h mainwindow.ui)

target_link_libraries(
  changer_gui PRIVATE project_options project_dependency Qt6::Core Qt6::Widgets
                      Qt6::Gui Qt6::Concurrent)

windeployqt(changer_gui)
