#include "wall_cache_library.h"
#include "json_helper.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace wallchanger {
cache_lib::cache_lib(bool load) {
  if (load) {
    if (deserialize()) {
      auto rng_it =
          ranges::find(m_cache_vec, m_active_name, &cache_store::first);
      if (rng_it != ranges::end(m_cache_vec)) {
        m_current = *rng_it;
      } else {
        m_current = m_cache_vec.front();
        m_active_name = m_current.first;
      }
    }
  }
}

void cache_lib::insert(std::string name, std::string path,
                       cache_lib_type value) noexcept {
  if (!exists(name)) {
    m_table.insert(std::move(path));
    m_cache_vec.emplace_back(std::move(name),
                             std::forward<cache_lib_type>(value));
  }
}

std::optional<cache_lib::cache_lib_cref>
cache_lib::get_cache(std::string_view name) const noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    return rng_it->second;
  }
  return {};
}

std::optional<cache_lib::cache_lib_ref>
cache_lib::get_cache(std::string_view name) noexcept {
  if (exists(name)) {
    auto itr = ranges::find(m_cache_vec, name, &cache_store::first);
    return itr->second;
  }
  return std::nullopt;
}

void cache_lib::change_active(std::string_view new_active) noexcept {
  if (exists(new_active)) {
    auto rng_it = ranges::find(m_cache_vec, new_active, &cache_store::first);
    m_active_name = rng_it->first;
    m_current = *rng_it;
  }
}

void cache_lib::rename_store(std::string_view from_name,
                             std::string_view to_name) noexcept {
  if (exists(from_name) && (!exists(to_name))) {
    auto rng_it = ranges::find(m_cache_vec, from_name, &cache_store::first);
    rng_it->first = to_name;
    if (from_name == m_active_name) {
      m_active_name = to_name;
    }
  }
}

void cache_lib::remove(std::string_view name) noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    rng_it->second.clear();
    m_clear_empty();
  }
}

bool cache_lib::is_empty() const noexcept { return m_cache_vec.empty(); }

bool cache_lib::exists(std::string_view name) const noexcept {
  if (!is_empty()) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    if (rng_it != ranges::end(m_cache_vec)) {
      return (rng_it->first == name);
    }
  }
  return false;
}

size_t cache_lib::capacity() const noexcept { return m_cache_vec.capacity(); }

size_t cache_lib::cache_count() const noexcept { return m_cache_vec.size(); }

std::string_view
cache_lib::get_cache_path(std::string_view name) const noexcept {
  if (exists(name)) {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    return {};
  }
  return {};
}

std::string cache_lib::cache_retrive_path(uint32_t id) const noexcept {
  if (auto dat = m_table.get(id)) {
    return dat.value().get().string();
  }

  return {};
}

bool cache_lib::modified() const noexcept {
  if (!is_empty()) {
    auto rng_it = ranges::find_if(
        m_cache_vec, [](auto &&type) { return type.second.modified(); });
    return rng_it->second.modified();
  }
  return false;
}

std::optional<cache_lib::cache_lib_cref>
cache_lib::get_current() const noexcept {
  return m_current.second;
}

std::string cache_lib::get_current_name() const noexcept {
  return m_active_name;
}

std::vector<std::string> cache_lib::cache_list() const noexcept {
  std::vector<std::string> ret(m_cache_vec.size());
  for (auto &&name : m_cache_vec) {
    ret.push_back(name.first);
  }
  return ret;
}

void cache_lib::serialize() const {
  if (modified()) {
    nlohmann::json obj;
    obj["total_count"] = m_cache_vec.size();
    obj["active"] = m_current.first;
    auto obj_array = nlohmann::json::array();
    for (auto &&entries : m_cache_vec) {
      obj_array.emplace_back(entries);
    }
    obj["cache_libraries"] = obj_array;

    std::ofstream obj_file(data_directory() + "/data/libraries.json",
                           std::ios::out);
    if (obj_file.good()) {
      obj_file << std::setw(4) << obj << "\n";
    }
    m_table.store();
  }
}

bool cache_lib::deserialize() {
  std::ifstream obj_file(data_directory() + "/data/libraries.json",
                         std::ios::in);
  if (obj_file.good()) {
    nlohmann::json obj;
    obj_file >> obj;
    if (!obj.is_null()) {
      for (auto &&entries : obj["cache_libraries"]) {
        m_cache_vec.emplace_back(entries.get<cache_store>());
      }
    }
    m_active_name = obj["active"].get<std::string>();
    return true;
  }
  return false;
}
} // namespace wallchanger