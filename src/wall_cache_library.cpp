#include "wall_cache_library.h"

wallchanger::cache_lib::cache_lib_type
wallchanger::cache_lib::get_cache(std::string_view name) const noexcept {
  auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
  return rng_it->second;
}

void wallchanger::cache_lib::remove(std::string_view name) noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    rng_it->second.clear();
    m_clear_empty();
  }
}

bool wallchanger::cache_lib::exists(std::string_view name) const noexcept {
  if (!empty()) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    return (rng_it->first == name);
  }
  return false;
}

bool wallchanger::cache_lib::modified() const noexcept {
  auto rng_it = ranges::find_if(
      m_cache_vec, [](auto &&type) { return type.second.modified(); });
  return rng_it->second.modified();
}
