#pragma once
#include "mainwindow.h"
#include <QApplication>

namespace wallchanger::gui {
class application : public QApplication {
  Q_OBJECT
public:
  explicit application(int argc, char *argv[]);
  int run();

private:
  std::unique_ptr<wallchanger::gui::MainWindow> m_main_window;
};
} // namespace wallchanger::gui