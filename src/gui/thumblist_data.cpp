#include "thumblist_data.h"

wallchanger::gui::ThumbListData::ThumbListData(QObject *parent)
    : QObject(parent) {}

bool wallchanger::gui::ThumbListData::insert(QString name, QImage image) {
  auto itr = ranges::find(m_cache, name, &thumb_cache_t::first);
  if (itr == ranges::end(m_cache)) {
    m_cache.emplaceBack(std::move(name), std::move(image));
    emit image_added(m_cache.back().first);
    return true;
  }
  return false;
}

void wallchanger::gui::ThumbListData::removeAt(int index) {
  m_cache.removeAt(index);
}

wallchanger::gui::thumb_cache_t
wallchanger::gui::ThumbListData::get_data_at(int idx) const {
  if (idx < 0 || idx >= m_cache.count()) {
    return {};
  }

  return m_cache.at(idx);
}

int wallchanger::gui::ThumbListData::count() const {
  if (!m_cache.empty()) {
    return static_cast<int>(m_cache.size());
  }
  return 0;
}

bool wallchanger::gui::ThumbListData::empty() const { return m_cache.empty(); }
