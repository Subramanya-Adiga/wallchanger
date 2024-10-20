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

void state::mark_favorate() noexcept {
  auto cur = m_previous.back();
  auto cur_cache = m_cache.get_current()->get();

  auto fnd_it =
      std::ranges::find(cur_cache, cur["wallpaper"].get<std::string_view>(),
                        &cache_lib::cache_lib_type::value_type::cache_value);
  fnd_it->cache_state = cache_state_e::favorate;
  LOG_INFO(m_logger, "Marked [{}] Wallpaper From [{}] As Favorate",
           cur["wallpaper"].get<std::string_view>(),
           m_cache.active_cache_name());
}

nlohmann::json state::get_next_wallpaper() noexcept {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  if (auto dat = m_cache.get_cache(m_cache.active_cache_name())) {
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
    send["collection"] = m_cache.active_cache_name();
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

bool state::change_active(const nlohmann::json &server_cmd,
                          uint32_t id) noexcept {
  auto name = server_cmd["new_active_name"].get<std::string>();
  if (m_cache.change_active(name)) {
    LOG_INFO(m_logger, "Client:[{}] changed active collection to:{} \n", id,
             m_cache.active_cache_name());
    return true;
  }
  return false;
}

bool state::rename_collection(const nlohmann::json &server_cmd) noexcept {
  if (m_cache.rename_store(server_cmd["col_name"].get<std::string>(),
                           server_cmd["col_name_new"].get<std::string>())) {
    LOG_INFO(m_logger, "created renamed:[{}] to:[{}]\n",
             server_cmd["col_name"].get<std::string>(),
             server_cmd["col_name_new"].get<std::string>());
    return true;
  }

  return false;
}

bool state::create_collection(const nlohmann::json &server_cmd) noexcept {
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
  if (m_cache.insert(server_cmd["new_col_name"], cache)) {
    LOG_INFO(m_logger, "created collection:[{}]\n",
             server_cmd["new_col_name"].get<std::string>());
    return true;
  }
  return false;
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
    LOG_INFO(m_logger, "added wall:[{}] to collection:[{}]\n",
             server_cmd["col_name"].get<std::string>(),
             server_cmd["wall"].get<std::string>());
    return true;
  }
  return false;
}

std::variant<std::monostate, cache_lib::cache_lib_cref,
             std::vector<std::string>>
state::list_collection(const nlohmann::json &server_cmd, uint32_t id) noexcept {
  if (!server_cmd["col_only"].get<bool>()) {
    LOG_INFO(m_logger, "client:[{}] requested to list collections\n", id);
    return m_cache.get_current().value();
  } else {
    LOG_INFO(m_logger, "client:[{}] requested to list collections\n", id);
    return m_cache.cache_list();
  }
  return {};
}

bool state::move_collectoion(const nlohmann::json &server_cmd,
                             uint32_t id) noexcept {

  auto col_frm = server_cmd["col_cur"].get<std::string_view>();
  auto col_to = server_cmd["col_new"].get<std::string_view>();
  auto wall = server_cmd["wall"].get<std::string_view>();

  if (m_cache.move_cache_item(col_frm, col_to, wall)) {
    LOG_INFO(m_logger,
             "client:[{}] requested to move wallpaper {} from {} to {}\n", id,
             wall, col_frm, col_to);
    return true;
  }
  return false;
}

bool state::merge_collection(const nlohmann::json &server_cmd,
                             uint32_t id) noexcept {
  auto col1 = server_cmd["col1"].get<std::string_view>();
  auto col2 = server_cmd["col2"].get<std::string_view>();
  if (m_cache.merge_cache(col1, col2)) {
    LOG_INFO(m_logger, "client:[{}] requested to merge collections {} {}\n", id,
             col1, col2);
    return true;
  }
  return false;
}

bool state::remove_collection(const nlohmann::json &server_cmd) noexcept {
  if (server_cmd["wall_only"].get<bool>()) {
    if (auto dat =
            m_cache.get_cache(server_cmd["col"].get<std::string_view>())) {

      LOG_INFO(m_logger, "removed wallpaper:[{}] from collection:[{}]\n",
               server_cmd["wall"].get<std::string_view>(),
               server_cmd["col"].get<std::string_view>());
      return true;
    }
  } else {
    if (m_cache.remove(server_cmd["col"].get<std::string_view>())) {
      LOG_INFO(m_logger, "removed collection:[{}]\n",
               server_cmd["col"].get<std::string_view>());
      return true;
    }
  }
  return false;
}

} // namespace wallchanger
