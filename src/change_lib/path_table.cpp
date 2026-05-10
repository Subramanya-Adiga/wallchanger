#include "path_table.hpp"
#include "crc32.hpp"
#include <filesystem>
#include <fstream>
#include "helpers.hpp"

namespace wallchanger {
path_table::path_table() {
  if (std::filesystem::exists(data_directory() + "/path_table.json")) {
    std::ifstream stream(data_directory() + "/path_table.json",
                         std::ios::in);
    if (stream.is_open()) {
      nlohmann::json obj;
      stream >> obj;
      if (!obj.is_null()) {
        m_store =
            obj["entries"]
                .get<std::vector<std::pair<u32, std::filesystem::path>>>();
      }
    }
  }
}

void path_table::insert(std::filesystem::path path) {
  auto p_str = path.string();
  auto hash = static_cast<u32>(
      wallchanger::helper::crc(p_str.begin(), p_str.end()));
  if (!exists(hash)) {
    m_modified = true;
    m_store.emplace_back(hash, std::move(path));
  }
}

std::optional<path_table::path_ref>
path_table::get(u32 id) const noexcept {
  if (exists(id)) {
    return std::ranges::find(m_store, id,
                             &std::pair<u32, std::filesystem::path>::first)
        ->second;
  }
  return std::nullopt;
}

bool path_table::exists(u32 id) const noexcept {
  auto itr = std::ranges::find(
      m_store, id, &std::pair<u32, std::filesystem::path>::first);
  return (itr != std::ranges::end(m_store));
}

void path_table::store() const noexcept {
  if (m_modified) {
    nlohmann::json obj;
    obj["entries"] = m_store;
    std::ofstream stream(data_directory() + "/path_table.json");
    stream << std::setw(4) << obj << "\n";
  }
}

} // namespace wallchanger
