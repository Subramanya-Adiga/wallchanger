#include "wall_app_gui.h"

wallchanger::gui::application::application(int argc, char *argv[])
    : QApplication(argc, argv),
      m_main_window(std::make_unique<wallchanger::gui::MainWindow>(nullptr)) {
  QCoreApplication::setApplicationName("changer_gui");
}

int wallchanger::gui::application::run() {
  m_main_window->show();
  return QApplication::exec();
}