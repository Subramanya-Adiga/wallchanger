#include "thumblist_data.h"

bool ThumbListData::insert(QString name, QImage image) {
  auto it = ranges::find(m_cache, name, &thumb_cache_t::first);
  if (it == ranges::end(m_cache)) {
    m_cache.emplaceBack(name, image);
    return true;
  }
  return false;
}

thumb_cache_t ThumbListData::get_data_at(int idx)const {
  if (idx == 0 || idx >= count())
    return {};

  return m_cache.at(idx);
}

int ThumbListData::count() const { return m_cache.count(); }
