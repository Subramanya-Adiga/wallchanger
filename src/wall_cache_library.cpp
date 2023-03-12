#include "wall_cache_library.h"
#include <fstream>
#include <nlohmann/json.hpp>

void wallchanger::cache_lib::insert(std::string name, std::string path,
                                    cache_lib_type value) noexcept {
  if (!exists(name)) {
    m_cache_vec.emplace_back(name, path, std::forward<cache_lib_type>(value));
  }
}

wallchanger::cache_lib::cache_lib_type
wallchanger::cache_lib::get_cache(std::string_view name) const noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
    m_current = *rng_it;
    return rng_it->cache;
  }
  return {};
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
    if (rng_it != ranges::end(m_cache_vec)) {
      return (rng_it->name == name);
    }
  }
  return false;
}

std::string_view
wallchanger::cache_lib::get_cache_path(std::string_view name) const noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
    return rng_it->path;
  }
  return "";
}

bool wallchanger::cache_lib::modified() const noexcept {
  if (!empty()) {
    auto rng_it = ranges::find_if(
        m_cache_vec, [](auto &&type) { return type.cache.modified(); });
    return rng_it->cache.modified();
  }
  return false;
}

void wallchanger::cache_lib::serialize() const {
  nlohmann::json obj;
  obj["total_count"] = m_cache_vec.size();
  auto obj_array = nlohmann::json::array();
  for (auto &&entries : m_cache_vec) {
    obj_array.push_back(entries);
  }
  obj["cache_libraries"] = obj_array;

  std::ofstream obj_file(data_directory() + "libraries.json", std::ios::out);
  if (obj_file.good()) {
    obj_file << std::setw(4) << obj << std::endl;
  }
}

void wallchanger::cache_lib::deserialize() {
  std::ifstream obj_file(data_directory() + "libraries.json", std::ios::in);
  if (obj_file.good()) {
    nlohmann::json obj;
    obj_file >> obj;
    if (!obj.is_null()) {
      for (auto &&entries : obj["cache_libraries"]) {
        m_cache_vec.push_back(entries.get<cache_store>());
      }
    }
  }
}