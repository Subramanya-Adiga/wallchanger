#include "wall_cache_library.h"
#include <type_traits>

wallchanger::cache_lib::cache_lib_type
wallchanger::cache_lib::get_cache(std::string_view name) const noexcept {
  auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
  m_current = *rng_it;
  return rng_it->cache;
}

void wallchanger::cache_lib::remove(std::string_view name) noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
    rng_it->cache.clear();
    m_clear_empty();
  }
}

bool wallchanger::cache_lib::exists(std::string_view name) const noexcept {
  if (!empty()) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
    return (rng_it->name == name);
  }
  return false;
}

bool wallchanger::cache_lib::modified() const noexcept {
  auto rng_it = ranges::find_if(
      m_cache_vec, [](auto &&type) { return type.cache.modified(); });
  return rng_it->cache.modified();
}
