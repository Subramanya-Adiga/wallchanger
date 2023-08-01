#include "thumblist_model.h"
#include <QFutureWatcher>
#include <QImageReader>
#include <QPixmap>
#include <QtConcurrent>

wallchanger::gui::ThumbListModel::ThumbListModel(const QString &path,
                                                 QObject *parent)
    : QAbstractListModel(parent),
      m_data(new wallchanger::gui::ThumbListData()) {

  connect(this, &wallchanger::gui::ThumbListModel::thumb_nail_ready, this,
          &wallchanger::gui::ThumbListModel::m_add_data);

  m_set_directory(path.toStdString());
}

int wallchanger::gui::ThumbListModel::rowCount(
    const QModelIndex &parent) const {
  return parent.isValid() ? 0 : m_data->count();
}

QVariant wallchanger::gui::ThumbListModel::data(const QModelIndex &index,
                                                int role) const {
  if (!index.isValid()) {
    return {};
  }

  const auto &data = m_data->get_data_at(index.row());

  if (role == Qt::DisplayRole) {
    return data.first;
  }

  if (role == Qt::DecorationRole) {
    return QPixmap::fromImage(data.second)
        .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return {};
  }

  return m_data->get_data_at(index.row()).first;
}

const wallchanger::gui::ThumbListData *
wallchanger::gui::ThumbListModel::get_model_data() const {
  return m_data.get();
}

void wallchanger::gui::ThumbListModel::m_add_data(QString name, QImage image) {
  beginInsertRows(QModelIndex(), m_counter, m_counter);
  (void)m_data->insert(std::move(name), std::move(image));
  endInsertRows();
  m_counter++;
}

void wallchanger::gui::ThumbListModel::m_set_directory(
    const std::filesystem::path &path) {
  if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {

    bool generate{true};
    auto *watcher = new QFutureWatcher<void>();

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            [&]() { generate = false; });

    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher] {
      watcher->deleteLater();
      emit directory_processed();
    });

    auto filesystem_range = ranges::make_subrange(
        std::filesystem::begin(std::filesystem::directory_iterator(path)),
        std::filesystem::end(std::filesystem::directory_iterator()));

    auto fs_vec = filesystem_range |
                  ranges::views::remove_if(
                      [](const auto &entry) { return entry.is_directory(); }) |
                  ranges::views::transform(
                      [](const auto &entry) { return entry.path(); }) |
                  ranges::to<QList>();

    QFuture<void> res;
    for (auto &&entries : fs_vec) {
      if (!generate) {
        break;
      }
      res = QtConcurrent::run(
          [this](const auto &entry) {
            QImage image(entry.string().c_str());
            QImageReader::setAllocationLimit(IMG_MEMORY_ALLOC);
            auto scaled = image.scaled({IMG_RES, IMG_RES}, Qt::KeepAspectRatio);
            scaled.convertTo(QImage::Format_ARGB32);
            emit thumb_nail_ready(entry.filename().string().c_str(), scaled);
          },
          entries);
    }
    watcher->setFuture(res);
  }
}
