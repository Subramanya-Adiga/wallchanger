#include "wall_cache_library.h"
#include "json_helper.h"
#include "wall_error.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace wallchanger {
cache_lib::cache_lib(bool load) {
  if (load) {
    if (deserialize()) {
      auto rng_it =
          std::ranges::find(m_cache_vec, m_active_name, &cache_store::first);
      if (rng_it != std::ranges::end(m_cache_vec)) {
        m_current = *rng_it;
      } else {
        m_current = m_cache_vec.front();
        m_active_name = m_current.first;
      }
    }
  }
}

void cache_lib::insert(std::string name, cache_lib_type value) noexcept {
  if (!exists(name)) {
    m_cache_vec.emplace_back(std::move(name),
                             std::forward<cache_lib_type>(value));
  }
}

std::optional<cache_lib::cache_lib_cref>
cache_lib::get_cache(std::string_view name) const noexcept {
  if (exists(name)) {
    auto rng_it = std::ranges::find(m_cache_vec, name, &cache_store::first);
    return rng_it->second;
  }
  return {};
}

std::optional<cache_lib::cache_lib_ref>
cache_lib::get_cache(std::string_view name) noexcept {
  if (exists(name)) {
    auto itr = std::ranges::find(m_cache_vec, name, &cache_store::first);
    return itr->second;
  }
  return std::nullopt;
}

void cache_lib::change_active(std::string_view new_active) noexcept {
  if (exists(new_active)) {
    auto rng_it =
        std::ranges::find(m_cache_vec, new_active, &cache_store::first);
    m_active_name = rng_it->first;
    m_current = *rng_it;
  }
}

void cache_lib::rename_store(std::string_view from_name,
                             std::string_view to_name) noexcept {
  if (exists(from_name) && (!exists(to_name))) {
    auto rng_it =
        std::ranges::find(m_cache_vec, from_name, &cache_store::first);
    rng_it->first = to_name;
    if (from_name == m_active_name) {
      m_active_name = to_name;
    }
  }
}

void cache_lib::remove(std::string_view name) noexcept {
  if (exists(name)) {
    auto rng_it = std::ranges::find(m_cache_vec, name, &cache_store::first);
    rng_it->second.clear();
    m_clear_empty();
  }
}

bool cache_lib::is_empty() const noexcept { return m_cache_vec.empty(); }

bool cache_lib::exists(std::string_view name) const noexcept {
  if (!is_empty()) {
    auto rng_it = std::ranges::find(m_cache_vec, name, &cache_store::first);
    if (rng_it != std::ranges::end(m_cache_vec)) {
      return (rng_it->first == name);
    }
  }
  return false;
}

size_t cache_lib::cache_count() const noexcept { return m_cache_vec.size(); }

bool wallchanger::cache_lib::modified() const noexcept {
  if (!is_empty()) {
    auto rng_it = std::ranges::find_if(
        m_cache_vec, [](auto &&type) { return type.second.modified(); });
    return rng_it->second.modified();
  }
  return false;
}

std::optional<cache_lib::cache_lib_cref>
cache_lib::get_current() const noexcept {
  return m_current.second;
}

std::vector<std::string> cache_lib::cache_list() const noexcept {
  std::vector<std::string> ret(m_cache_vec.size());
  for (auto &&name : m_cache_vec) {
    ret.push_back(name.first);
  }
  return ret;
}

outcome::result<void> cache_lib::merge_cache(std::string_view col1,
                                             std::string_view col2) noexcept {
  if (col1 != col2) {
    if (exists(col1) && exists(col2)) {
      auto col1_it = std::ranges::find(m_cache_vec, col1, &cache_store::first);
      auto col2_it = std::ranges::find(m_cache_vec, col2, &cache_store::first);

      cache_store new_store(col1.data(),
                            (col1_it->second.size() + col2_it->second.size()));

      std::ranges::merge(col1_it->second, col2_it->second,
                         new_store.second.begin());
      remove(col1);
      remove(col2);
      m_cache_vec.emplace_back(new_store);
    }
    return wall_errc::cache_does_not_exists;
  }
  return wall_errc::cache_frm_cache_to_same;
}

outcome::result<void>
cache_lib::move_cache_item(std::string_view source, std::string_view dest,
                           std::string_view item_name) noexcept {
  if ((source != dest) && (dest != item_name)) {
    if (exists(source) && exists(dest)) {
      auto src = std::ranges::find(m_cache_vec, source, &cache_store::first);
      auto dst = std::ranges::find(m_cache_vec, dest, &cache_store::first);

      if (src->second.contains(item_name.data())) {

        auto itm_itr = std::ranges::find(src->second, item_name,
                                         &cache_type::value_type::cache_value);

        dst->second.insert_elem(std::move(*itm_itr));
        src->second.erase(itm_itr);
      }
      return wall_errc::cache_elem_not_exists;
    }
    return wall_errc::cache_does_not_exists;
  }
  return wall_errc::cache_frm_cache_to_same;
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
