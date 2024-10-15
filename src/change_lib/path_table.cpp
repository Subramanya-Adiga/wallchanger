#include "path_table.hpp"
#include "net/net_common.hpp"
#include <fstream>

namespace wallchanger {
path_table::path_table() {
  if (std::filesystem::exists(data_directory() + "/data/path_table.json")) {
    std::ifstream stream(data_directory() + "/data/path_table.json",
                         std::ios::in);
    if (stream.is_open()) {
      nlohmann::json obj;
      stream >> obj;
      if (!obj.is_null()) {
        for (auto &&entries : obj["entries"]) {
          m_store.emplace_back(
              entries.get<std::pair<uint32_t, std::filesystem::path>>());
        }
      }
    }
  }
}

void path_table::insert(std::filesystem::path path) {
  auto p_str = path.string();
  auto hash = static_cast<uint32_t>(
      wallchanger::helper::crc(p_str.begin(), p_str.end()));
  if (!exists(hash)) {
    m_modified = true;
    m_store.emplace_back(hash, std::move(path));
  }
}

std::optional<path_table::path_ref>
path_table::get(uint32_t id) const noexcept {
  if (exists(id)) {
    return std::ranges::find(m_store, id,
                             &std::pair<uint32_t, std::filesystem::path>::first)
        ->second;
  }
  return std::nullopt;
}

bool path_table::exists(uint32_t id) const noexcept {
  auto itr = std::ranges::find(
      m_store, id, &std::pair<uint32_t, std::filesystem::path>::first);
  return (itr != std::ranges::end(m_store));
}

void path_table::store() const noexcept {
  if (m_modified) {
    nlohmann::json obj;
    auto obj_array = nlohmann::json::array();
    for (auto &&elem : m_store) {
      obj_array.push_back(elem);
    }
    obj["entries"] = obj_array;
    std::ofstream stream(data_directory() + "/data/path_table.json");
    stream << std::setw(4) << obj << "\n";
  }
}

} // namespace wallchanger