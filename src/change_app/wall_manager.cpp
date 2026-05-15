#include "wall_manager.hpp"
#include "../log.hpp"
#include "helpers.hpp"
#include <crc32.hpp>
#include <fstream>

namespace wallchanger {
Manager::Manager(std::string_view logger_name)
    : m_logger(logger_name), m_cache(true) {}

void Manager::store() {
  if (!m_previous.empty()) {
    std::ofstream hist(data_directory() + "/history.json", std::ios::out);
    nlohmann::json obj;
    obj["histoy"] = m_previous;
    hist << std::setw(4) << obj << "\n";
  }
  m_path_buf.store();
  m_cache.serialize();
}

void Manager::mark_favorate() {
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

nlohmann::json Manager::next_wall() {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  auto dat = m_cache.get_cache(m_cache.active_cache_name());
  if (!dat) {
    return {};
  }
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

nlohmann::json Manager::previous_wall() {
  if (!m_previous.empty()) {
    auto previous = m_previous.back();
    m_previous.pop_back();
    return previous;
  }
  return {};
}

bool Manager::change_active(std::string_view cmd) noexcept {
  if (m_cache.change_active(cmd)) {
    LOG_INFO(m_logger, "Changed active collection to:{} \n",
             m_cache.active_cache_name());
    return true;
  }
  return false;
}

bool Manager::rename_collection(const nlohmann::json &cmd) noexcept {
  if (m_cache.rename_store(cmd["col_name"].get<std::string>(),
                           cmd["col_name_new"].get<std::string>())) {
    LOG_INFO(m_logger, "created renamed:[{}] to:[{}]\n",
             cmd["col_name"].get<std::string>(),
             cmd["col_name_new"].get<std::string>());
    return true;
  }

  return false;
}

bool Manager::create_collection(const nlohmann::json &cmd) noexcept {
  wallchanger::cache_lib::cache_lib_type cache;

  if (!cmd["col_empty"].get<bool>()) {
    auto col_path = cmd["col_path"].get<std::string>();
    auto crc_loc = static_cast<uint32_t>(
        wallchanger::helper::crc(col_path.begin(), col_path.end()));

    auto inserter = [&](const std::filesystem::directory_entry &path) {
      if (!path.is_directory()) {
        cache.insert(path.path().filename().string(), crc_loc);
      }
    };

    if (!cmd["recursive"].get<bool>()) {
      std::ranges::for_each(std::filesystem::directory_iterator(col_path),
                            inserter);
    } else {
      std::ranges::for_each(
          std::filesystem::recursive_directory_iterator(col_path), inserter);
    }

    m_path_buf.insert(col_path);
    LOG_INFO(m_logger, "created collection:[{}] path:[{}]\n",
             cmd["new_col_name"].get<std::string>(),
             cmd["col_path"].get<std::string>());
  }
  if (m_cache.insert(cmd["new_col_name"], cache)) {
    LOG_INFO(m_logger, "created collection:[{}]\n",
             cmd["new_col_name"].get<std::string>());
    return true;
  }
  return false;
}

bool Manager::add_to_collection(const nlohmann::json &cmd) noexcept {
  auto col_name = cmd["col_name"].get<std::string>();
  auto wall = cmd["wall"].get<std::filesystem::path>();

  auto wall_path = wall.parent_path().string();
  auto path_crc = static_cast<uint32_t>(
      wallchanger::helper::crc(wall_path.begin(), wall_path.end()));
  if (auto dat = m_cache.get_cache(col_name)) {
    auto &cache = dat.value().get();

    cache.insert(wall.filename().string(), path_crc);
    m_path_buf.insert(wall_path);
    LOG_INFO(m_logger, "added wall:[{}] to collection:[{}]\n",
             cmd["col_name"].get<std::string>(),
             cmd["wall"].get<std::string>());
    return true;
  }
  return false;
}

std::variant<std::monostate, cache_lib::cache_lib_cref,
             std::vector<std::string>>
Manager::list_collection(const nlohmann::json &cmd) noexcept {
  if (!cmd["col_only"].get<bool>()) {
    LOG_INFO(m_logger, "requested to list collections\n");
    return m_cache.get_current().value();
  } else {
    LOG_INFO(m_logger, "requested to list collections\n");
    return m_cache.cache_list();
  }
  return {};
}

bool Manager::move_collectoion(const nlohmann::json &cmd) noexcept {
  auto col_frm = cmd["col_cur"].get<std::string_view>();
  auto col_to = cmd["col_new"].get<std::string_view>();
  auto wall = cmd["wall"].get<std::string_view>();

  if (m_cache.move_cache_item(col_frm, col_to, wall)) {
    LOG_INFO(m_logger, "requested to move wallpaper {} from {} to {}\n", wall,
             col_frm, col_to);
    return true;
  }
  return false;
}

bool Manager::merge_collection(const nlohmann::json &cmd) noexcept {
  auto col1 = cmd["col1"].get<std::string_view>();
  auto col2 = cmd["col2"].get<std::string_view>();
  if (m_cache.merge_cache(col1, col2)) {
    LOG_INFO(m_logger, "requested to merge collections {} {}\n", col1, col2);
    return true;
  }
  return false;
}

bool Manager::remove_collection(const nlohmann::json &cmd) noexcept {
  if (cmd["wall_only"].get<bool>()) {
    if (auto dat = m_cache.get_cache(cmd["col"].get<std::string_view>())) {

      LOG_INFO(m_logger, "removed wallpaper:[{}] from collection:[{}]\n",
               cmd["wall"].get<std::string_view>(),
               cmd["col"].get<std::string_view>());
      return true;
    }
  } else {
    if (m_cache.remove(cmd["col"].get<std::string_view>())) {
      LOG_INFO(m_logger, "removed collection:[{}]\n",
               cmd["col"].get<std::string_view>());
      return true;
    }
  }
  return false;
}
} // namespace wallchanger
