#include "mainwindow.h"
#include <QApplication>
#include "../platform/windows_helper.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  QApplication app(argc, argv);
  ENABLE_CRT_NEW
  QCoreApplication::setApplicationName("changer_gui");
  auto mainwindow = std::make_unique<MainWindow>(nullptr);
  mainwindow->show();
  return QCoreApplication::exec();
}
