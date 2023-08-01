#pragma once
#include <QImage>
#include <QObject>
#include <QString>

namespace wallchanger::gui {

using thumb_cache_t = std::pair<QString, QImage>;
using thumb_cache_list_t = QList<thumb_cache_t>;

class ThumbListData : public QObject {
  Q_OBJECT
public:
  explicit ThumbListData(QObject *parent = nullptr);
  [[nodiscard]] bool insert(QString name, QImage image);
  void removeAt(int index);
  [[nodiscard]] thumb_cache_t get_data_at(int idx) const;
  [[nodiscard]] int count() const;
  [[nodiscard]] bool empty() const;

signals:
  void image_added(const QString &);

private:
  thumb_cache_list_t m_cache;
};

} // namespace wallchanger::gui
