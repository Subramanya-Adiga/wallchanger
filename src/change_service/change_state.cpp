#include "change_state.hpp"
#include <fstream>
#include <net/net_common.hpp>
#include <random>

namespace wallchanger {

state::state(std::string_view logger) : m_logger(logger), m_cache(true) {}

void state::store_state() noexcept {
  if (!m_previous.empty()) {
    std::ofstream hist(data_directory() + "/data/history.json", std::ios::out);
    nlohmann::json obj;
    obj["histoy"] = m_previous;
    hist << std::setw(4) << obj << "\n";
  }
  m_path_buf.store();
  m_cache.serialize();
}

nlohmann::json state::get_next_wallpaper() noexcept {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  if (auto dat = m_cache.get_cache(m_active)) {
    auto cache = dat.value().get();
    std::uniform_int_distribution<> dist(1, static_cast<int>(cache.size()));

    bool found = false;
    cache_lib::cache_lib_type::value_type ret{};
    size_t idx{};
    while (!found) {
      idx = static_cast<size_t>(dist(generator));
      if (auto state = cache[idx].cache_state;
          state == wallchanger::cache_state_e::unused) {
        ret = cache[idx];
        cache[idx].cache_state = wallchanger::cache_state_e::used;
        found = true;
      }
    }

    uint32_t path_loc = cache[idx].loc;
    nlohmann::json send;
    send["wallpaper"] = ret.cache_value;
    send["path"] = m_path_buf.get(path_loc).value().get();
    send["index"] = idx;
    send["collection"] = m_active;
    m_previous.push_back(send);
    return send;
  }
}

nlohmann::json state::get_previous_wallpaper() noexcept {
  if (!m_previous.empty()) {
    auto previous = m_previous.back();
    m_previous.pop_back();
    return previous;
  }
  return {};
}

void state::create_collection(const nlohmann::json &server_cmd) noexcept {
  wallchanger::cache_lib::cache_lib_type cache;

  if (!server_cmd["col_empty"].get<bool>()) {
    auto col_path = server_cmd["col_path"].get<std::string>();
    auto crc_loc = static_cast<uint32_t>(
        wallchanger::helper::crc(col_path.begin(), col_path.end()));

    auto inserter = [&](const std::filesystem::directory_entry &path) {
      if (!path.is_directory()) {
        cache.insert(path.path().filename().string(), crc_loc);
      }
    };

    if (!server_cmd["recursive"].get<bool>()) {
      std::ranges::for_each(std::filesystem::directory_iterator(col_path),
                            inserter);
    } else {
      std::ranges::for_each(
          std::filesystem::recursive_directory_iterator(col_path), inserter);
    }

    m_path_buf.insert(col_path);
    LOG_INFO(m_logger, "created collection:[{}] path:[{}]\n",
             server_cmd["new_col_name"].get<std::string>(),
             server_cmd["col_path"].get<std::string>());
  }
  m_cache.insert(server_cmd["new_col_name"], cache);
}

bool state::add_to_collection(const nlohmann::json &server_cmd) noexcept {
  auto col_name = server_cmd["col_name"].get<std::string>();
  auto wall = server_cmd["wall"].get<std::filesystem::path>();

  auto wall_path = wall.parent_path().string();
  auto path_crc = static_cast<uint32_t>(
      wallchanger::helper::crc(wall_path.begin(), wall_path.end()));
  if (auto dat = m_cache.get_cache(col_name)) {
    auto &cache = dat.value().get();

    cache.insert(wall.filename().string(), path_crc);
    m_path_buf.insert(wall_path);
    return true;
  }
  return false;
}

} // namespace wallchanger
