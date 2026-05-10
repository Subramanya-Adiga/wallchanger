#include "wall_manager.hpp"
#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include "helpers.hpp"
#include "../log.hpp"

namespace wallchanger {
manager::manager(std::string_view logger_name)
    : m_logger(logger_name), m_cache(true) {}

void manager::store() {
  if (!m_previous.empty()) {
    std::ofstream hist(data_directory() + "/history.json", std::ios::out);
    nlohmann::json obj;
    obj["histoy"] = m_previous;
    hist << std::setw(4) << obj << "\n";
  }
  m_path_buf.store();
  m_cache.serialize();
}

void manager::mark_favorate() {
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

nlohmann::json manager::next_wall() {
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

nlohmann::json manager::previous_wall(){
  if (!m_previous.empty()) {
    auto previous = m_previous.back();
    m_previous.pop_back();
    return previous;
  }
  return {};
}
} // namespace wallchanger
