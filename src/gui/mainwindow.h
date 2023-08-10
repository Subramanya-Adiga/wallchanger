#pragma once

#include "ui_mainwindow.h"
#include <QMainWindow>

namespace wallchanger::gui {
class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void m_item_selected(const QModelIndex &index);
};
} // namespace wallchanger::gui