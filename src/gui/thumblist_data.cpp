#include "thumblist_data.h"
#include <QImageReader>
#include <QtConcurrent>

wallchanger::gui::ThumbListData::ThumbListData(QObject *parent)
    : QObject(parent), m_cache(new wallchanger::gui::thumb_cache_list_t()) {
  connect(this, &wallchanger::gui::ThumbListData::thumb_nail_ready, this,
          &wallchanger::gui::ThumbListData::insert);
}

bool wallchanger::gui::ThumbListData::insert(QString name, QImage image) {
  auto itr = ranges::find(*m_cache, name, &thumb_cache_t::first);
  if (itr == ranges::end(*m_cache)) {
    m_cache->emplaceBack(std::move(name), std::move(image));
    emit image_added(m_cache->back().first);
    return true;
  }
  return false;
}

wallchanger::gui::thumb_cache_t
wallchanger::gui::ThumbListData::get_data_at(int idx) const {
  if (idx == 0 || idx >= count()) {
    return {};
  }

  return m_cache->at(idx);
}

int wallchanger::gui::ThumbListData::count() const {
  if (!m_cache->empty()) {
    return static_cast<int>(m_cache->size());
  }
  return 0;
}

void wallchanger::gui::ThumbListData::set_directory(
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
      if (generate) {
        res = QtConcurrent::run(
            [this](const auto &entry) {
              QImage image(entry.string().c_str());
              QImageReader::setAllocationLimit(IMG_MEMORY_ALLOC);
              auto scaled =
                  image.scaled({IMG_RES, IMG_RES}, Qt::KeepAspectRatio);
              scaled.convertTo(QImage::Format_ARGB32);
              emit thumb_nail_ready(entry.filename().string().c_str(), scaled);
            },
            entries);
      }
    }
    watcher->setFuture(res);
  }
}

static void write_to_file(QImage scaled, QString name) {
  QDir thumbdir(QCoreApplication::applicationDirPath() + "/thumbnails");
  if (!thumbdir.exists()) {
    thumbdir.mkdir(QCoreApplication::applicationDirPath() + "/thumbnails");
  }
  scaled.save(thumbdir.path() + "/" + name, "PNG");
}