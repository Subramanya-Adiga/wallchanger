#include "mainwindow.h"
#include <QColorSpace>
#include <QImageReader>
#include <QThreadPool>
#include <QtConcurrent>
#include <filesystem>
#include "thumblist_model.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  QObject::connect(this, &MainWindow::add_item, this,
                   &MainWindow::add_to_item_list,Qt::DirectConnection);
  auto* model = new ThumbListModel(this);
  m_threadded_add();
  ImageList->setModel(model);
}

MainWindow::~MainWindow() { m_close = true; }

void MainWindow::add_to_item_list(QImage ico, QString name) {
}

void MainWindow::m_threadded_add() {
  QFuture<void> fut;
  auto pool = new QThreadPool();
  for (auto &&entry : std::filesystem::directory_iterator("D:/wallpaper")) {
    if (!m_close) {
      fut = QtConcurrent::run(pool,
          [this](std::filesystem::directory_entry in_path) {
            QImage image(in_path.path().string().c_str());
            QImageReader::setAllocationLimit(1024);
            auto scaled = image.scaled({64, 64}, Qt::KeepAspectRatio);
            scaled.convertTo(QImage::Format_ARGB32);
            QDir thumbdir(QCoreApplication::applicationDirPath() +
                          "/thumbnails");
            if (!thumbdir.exists()) {
              thumbdir.mkdir(QCoreApplication::applicationDirPath() +
                             "/thumbnails");
            }
            scaled.save(thumbdir.path() + "/" +
                            in_path.path().filename().string().c_str(),
                        "PNG");
            emit add_item(scaled, in_path.path().string().c_str());
          },
          entry);
     // fut.waitForFinished();
    }
  }
}
