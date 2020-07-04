#include "wall_cache_library.h"

wall_changer::wall_cache_lib::value_type
wall_changer::wall_cache_lib::get(std::string_view name) const noexcept {
  auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
  return rng_it->second;
}

void wall_changer::wall_cache_lib::erase(std::string_view name) noexcept {
  auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
  if (rng_it->first == name) {
    rng_it->second.clear();
    m_clear_empty();
  }
}

bool wall_changer::wall_cache_lib::exists(
    std::string_view name) const noexcept {
  auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
  return (rng_it->first == name);
}

bool wall_changer::wall_cache_lib::modified() const noexcept {
  auto rng_it = ranges::find_if(
      m_cache_vec, [](auto &&type) { return type.second.modified(); });
  return rng_it->second.modified();
}
