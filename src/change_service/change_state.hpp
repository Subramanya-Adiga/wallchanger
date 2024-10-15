#pragma once
#include <path_table.hpp>
#include <wall_cache_library.hpp>

namespace wallchanger {

class state {
public:
  explicit state(std::string_view logger_name);
  void store_state() noexcept;

  nlohmann::json get_next_wallpaper() noexcept;
  nlohmann::json get_previous_wallpaper() noexcept;

  std::string_view change_active(std::string_view col) noexcept;

  void create_collection(const nlohmann::json &server_cmd) noexcept;
  bool add_to_collection(const nlohmann::json &server_cmd) noexcept;

private:
  std::string_view m_logger;
  wallchanger::cache_lib m_cache;
  path_table m_path_buf;
  std::vector<nlohmann::json> m_previous;
  std::string m_active;
};
} // namespace wallchanger
