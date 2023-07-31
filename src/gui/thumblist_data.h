#pragma once
#include <QImage>
#include <QObject>
#include <QString>

namespace wallchanger::gui {
static constexpr int IMG_MEMORY_ALLOC = 1024;
static constexpr int IMG_RES = 256;

using thumb_cache_t = std::pair<QString, QImage>;
using thumb_cache_list_t = QList<thumb_cache_t>;

class ThumbListData : public QObject {
  Q_OBJECT
public:
  explicit ThumbListData(QObject *parent = nullptr);
  void set_directory(const std::filesystem::path &path);
  [[nodiscard]] bool insert(QString name, QImage image);
  [[nodiscard]] thumb_cache_t get_data_at(int idx) const;
  [[nodiscard]] int count() const;

signals:
  void thumb_nail_ready(QString, QImage);
  void directory_processed();
  void image_added(const QString &);

private:
  QScopedPointer<thumb_cache_list_t> m_cache;
};

} // namespace wallchanger::gui
