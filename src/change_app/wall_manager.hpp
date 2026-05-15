#pragma once
#include "path_table.hpp"
#include "wall_cache_library.hpp"

namespace wallchanger {

class Manager {
public:
  explicit Manager(std::string_view logger_name);
  void store();
  void mark_favorate();
  [[nodiscard]] nlohmann::json next_wall();
  [[nodiscard]] nlohmann::json previous_wall();

  [[nodiscard]] bool change_active(std::string_view cmd) noexcept;

  [[nodiscard]] bool rename_collection(const nlohmann::json &cmd) noexcept;

  [[nodiscard]] bool create_collection(const nlohmann::json &cmd) noexcept;
  [[nodiscard]] bool add_to_collection(const nlohmann::json &cmd) noexcept;

  [[nodiscard]] std::variant<std::monostate, cache_lib::cache_lib_cref,
                             std::vector<std::string>>
  list_collection(const nlohmann::json &cmd) noexcept;

  [[nodiscard]] bool move_collectoion(const nlohmann::json &cmd) noexcept;
  [[nodiscard]] bool merge_collection(const nlohmann::json &cmd) noexcept;

  [[nodiscard]] bool remove_collection(const nlohmann::json &cmd) noexcept;

private:
  std::string_view m_logger;
  wallchanger::cache_lib m_cache;
  wallchanger::path_table m_path_buf;
  std::vector<nlohmann::json> m_previous;
};

} // namespace wallchanger
