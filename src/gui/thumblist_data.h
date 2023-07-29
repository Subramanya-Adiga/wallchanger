#pragma once

#include <QHash>
#include <QImage>

using thumb_cache_t = std::pair<QString, QImage>;
using thumb_cache_list_t = QList<thumb_cache_t>;

class ThumbListData {
public:
  ThumbListData() = default;
  bool insert(QString name, QImage image);
  thumb_cache_t get_data_at(int idx)const;
  int count() const;

private:
  thumb_cache_list_t m_cache;
};
