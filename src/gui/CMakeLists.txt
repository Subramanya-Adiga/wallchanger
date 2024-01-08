include(windeployqt)

add_executable(changer_gui WIN32 gui_main.cpp)

set_target_properties(
  changer_gui
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

target_precompile_headers(
  changer_gui
  PRIVATE
  <QObject>
  <QApplication>
  <QListView>
  <QAbstractListModel>
  <QPixmap>
  <QImage>
  <QString>
  <QList>
  <QMainWindow>
  <QMessageBox>
  <QtConcurrent>)

target_sources(
  changer_gui
  PRIVATE wall_app_gui.h
          wall_app_gui.cpp
          wall_app_qml.h
          wall_app_qml.cpp
          mainwindow.cpp
          mainwindow.h
          mainwindow.ui
          thumblist_model.h
          thumblist_model.cpp
          thumblist_data.h
          thumblist_data.cpp
          resources.qrc)

target_link_libraries(
  changer_gui
  PRIVATE project_options
          project_dependency
          Qt6::Core
          Qt6::Widgets
          Qt6::Gui
          Qt6::Quick
          Qt6::QuickControls2
          Qt6::Concurrent)

windeployqt(changer_gui)
