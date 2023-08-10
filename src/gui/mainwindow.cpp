#include "mainwindow.h"
#include "thumblist_data.h"
#include "thumblist_model.h"
#include <QLabel>

wallchanger::gui::MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

  setupUi(this);

  listView->setGridSize({132, 132});
  listView->setResizeMode(QListView::Adjust);
  listView->setWrapping(true);

  listView->setModel(new wallchanger::gui::ThumbListModel("D:/wallpaper"));
  auto *model =
      qobject_cast<wallchanger::gui::ThumbListModel *>(listView->model());
  auto *lbl = new QLabel("", this);

  connect(model->get_model_data(),
          &wallchanger::gui::ThumbListData::image_added, lbl, &QLabel::setText);
  connect(model, &wallchanger::gui::ThumbListModel::directory_processed,
          listView, &QListView::reset);
  connect(listView, &QAbstractItemView::clicked, this,
          &MainWindow ::m_item_selected);

  statusbar->addWidget(lbl);
}

void wallchanger::gui::MainWindow::m_item_selected(const QModelIndex &index) {
  auto *model =
      qobject_cast<wallchanger::gui::ThumbListModel *>(listView->model());
  auto name = model->get_model_data()->get_data_at(index.row()).first;
  auto path = model->get_model_data_path();
  auto pixmap = QPixmap(path + "/" + name);
  auto *pixmap_label = new QLabel(this);
  pixmap_label->setPixmap(pixmap);
  scrollArea->setWidget(pixmap_label);
}