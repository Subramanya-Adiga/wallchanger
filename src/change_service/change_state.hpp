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

  void mark_favorate() noexcept;

  [[nodiscard]] bool change_active(const nlohmann::json &server_cmd,
                                   uint32_t id) noexcept;

  [[nodiscard]] bool
  rename_collection(const nlohmann::json &server_cmd) noexcept;

  [[nodiscard]] bool
  create_collection(const nlohmann::json &server_cmd) noexcept;
  [[nodiscard]] bool
  add_to_collection(const nlohmann::json &server_cmd) noexcept;

  [[nodiscard]] std::variant<std::monostate, cache_lib::cache_lib_cref,
                             std::vector<std::string>>
  list_collection(const nlohmann::json &server_cmd, uint32_t id) noexcept;

  [[nodiscard]] bool move_collectoion(const nlohmann::json &server_cmd,
                                      uint32_t id) noexcept;
  [[nodiscard]] bool merge_collection(const nlohmann::json &server_cmd,
                                      uint32_t id) noexcept;

  [[nodiscard]] bool
  remove_collection(const nlohmann::json &server_cmd) noexcept;

private:
  std::string_view m_logger;
  wallchanger::cache_lib m_cache;
  path_table m_path_buf;
  std::vector<nlohmann::json> m_previous;
};
} // namespace wallchanger
