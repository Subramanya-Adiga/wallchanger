#include "wall_manager.hpp"
#include "../log.hpp"
#include "helpers.hpp"
#include "wall_cache_library.hpp"
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
  if (!m_previous.empty()) {
    auto cur = m_previous.back();
    auto cur_cache = m_cache.get_current();

    auto fnd_it =
        std::ranges::find(cur_cache, cur["wallpaper"].get<std::string_view>(),
                          &cache_lib::cache_lib_type::value_type::cache_value);
    fnd_it->cache_state = cache_state_e::favorate;

    LOG_INFO(m_logger, "Marked [{}] Wallpaper From [{}] As Favorate",
             cur["wallpaper"].get<std::string_view>(),
             m_cache.active_cache_name());
  }
}

nlohmann::json Manager::next_wall() {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  auto dat = m_cache.get_cache(m_cache.active_cache_name());
  if (!dat) {
    LOG_ERR(m_logger,
            "Error Qccuren While Accessing Active "
            "Collection.\nErrorCode:{}\nErrorMessage:{}\n",
            dat.error().value(), dat.error().message());
    return {};
  }
  auto cache = dat.value();
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

  u32 path_loc = cache[idx].loc;
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
  if (auto res = m_cache.change_active(cmd); res.has_value()) {
    LOG_INFO(m_logger, "Changed active collection to:{} \n",
             m_cache.active_cache_name());
    return true;
  } else {
    LOG_ERR(m_logger,
            "Error Occured While Chainging Active "
            "Collection.\nErrorCode:{}\n,ErrorMessage:{}\n",
            res.error().value(), res.error().message());
  }
  return false;
}

bool Manager::rename_collection(std::string_view from,
                                std::string_view to) noexcept {
  if (auto res = m_cache.rename_store(from, to); res.has_value()) {
    LOG_INFO(m_logger, "created renamed:[{}] to:[{}]\n", from, to);
    return true;
  } else {
    LOG_ERR(m_logger,
            "Error Occured While Renaming Collection."
            "From:{} To:{}\nErrorCode:{}\nErrorMessage:{}\n",
            from, to, res.error().value(), res.error().message());
  }
  return false;
}

bool Manager::create_collection(std::string name,
                                const std::filesystem::path &path,
                                bool recursive) noexcept {
  wallchanger::cache_lib::cache_lib_type cache;

  if ((!path.empty()) && std::filesystem::exists(path)) {
    auto col_path = path.string();
    auto crc_loc = static_cast<u32>(
        wallchanger::helper::crc(col_path.begin(), col_path.end()));

    auto inserter = [&](const std::filesystem::directory_entry &path_itr) {
      if (!path_itr.is_directory()) {
        cache.insert(path_itr.path().filename().string(), crc_loc);
      }
    };

    if (!recursive) {
      std::ranges::for_each(std::filesystem::directory_iterator(col_path),
                            inserter);
    } else {
      std::ranges::for_each(
          std::filesystem::recursive_directory_iterator(col_path), inserter);
    }

    m_path_buf.insert(col_path);
    LOG_INFO(m_logger, "created collection:[{}] path:[{}]\n", name, col_path);
  }
  if (auto res = m_cache.insert(name, cache); res.has_value()) {
    return true;
  } else {
    LOG_ERR(
        m_logger,
        "Error While Creataing Collection.\nErrorCode:{}\nErrorMessage:{}\n",
        res.error().value(), res.error().message());
  }
  return false;
}

bool Manager::add_to_collection(std::string_view collection_name,
                                const std::filesystem::path &wall) noexcept {

  auto wall_path = wall.parent_path().string();
  auto path_crc = static_cast<u32>(
      wallchanger::helper::crc(wall_path.begin(), wall_path.end()));

  if (m_cache.exists(collection_name)) {
    auto &cache = m_cache[collection_name];
    cache.insert(wall.filename().string(), path_crc);
    m_path_buf.insert(wall_path);
    LOG_INFO(m_logger, "added wall:[{}] to collection:[{}]\n", wall.string(),
             collection_name);
    return true;
  }
  return false;
}

std::vector<std::string> Manager::list_collection() const noexcept {
  return m_cache.cache_list();
}

cache_lib::const_slice
Manager::get_cache(std::string_view name) const {
  if(auto res = m_cache.get_cache(name);res.has_value()){
    return res.value();
  }else{
    LOG_ERR(m_logger,
            "Error Qccuren While Accessing Active "
            "Collection.\nErrorCode:{}\nErrorMessage:{}\n",
            res.error().value(), res.error().message());
  }
  return {};
}

bool Manager::merge_collection(std::string_view collection_1,
                               std::string_view collection_2) noexcept {
  if (auto res = m_cache.merge_cache(collection_1, collection_2);
      res.has_value()) {
    LOG_INFO(m_logger, "Merged Collections {} {}\n", collection_1,
             collection_2);
    return true;
  } else {
    LOG_ERR(m_logger,
            "Error Occured Merging Collections. {} With "
            "{}.\nErroCode:{}\nErrorMessage:{}\n",
            collection_1, collection_2, res.error().value(),
            res.error().message());
  }
  return false;
}

bool Manager::remove_collection(std::string_view collection) noexcept {
  if (auto res = m_cache.remove(collection); res.has_value()) {
    LOG_INFO(m_logger, "removed collection:[{}]\n", collection);
    return true;
  } else {
    LOG_ERR(m_logger,
            "Error Occured Removing Collection "
            "{}.\nErrorCode:{}\nErrorMessage:{}\n",
            collection, res.error().value(), res.error().message());
  }
  return false;
}

bool Manager::move_wallpaper(std::string_view origin_collection,
                             std::string_view dest_collection,
                             std::string_view wall_name) noexcept {

  if (auto res = m_cache.move_cache_item(origin_collection, dest_collection,
                                         wall_name);
      res.has_value()) {
    LOG_INFO(m_logger,
             "Moved Wallpaper {} From Origin Collection:{} To Destination "
             "Collection:{}\n",
             wall_name, origin_collection, dest_collection);
    return true;
  } else {
    LOG_ERR(m_logger,
            "Error Occured Moving Wallpaper:{} From Collection:{} To "
            "Collection:{}.\nErrorCode:{}\nErrorMessage:{}\n",
            wall_name, origin_collection, dest_collection, res.error().value(),
            res.error().message());
  }
  return false;
}
} // namespace wallchanger
