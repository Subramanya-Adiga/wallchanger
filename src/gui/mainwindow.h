#pragma once

#include "qfuture.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QFuture>
#include <atomic>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow()override;

signals:
  void add_item(QImage ico,QString name);

public slots:
  void add_to_item_list(QImage ico,QString name);

private:
  QFuture<void>  m_add_res;
  std::atomic_bool m_close{false};
  void m_threadded_add();
};
