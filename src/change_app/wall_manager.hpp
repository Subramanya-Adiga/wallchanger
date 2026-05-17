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

  [[nodiscard]] bool rename_collection(std::string_view from,
                                       std::string_view to) noexcept;

  [[nodiscard]] bool create_collection(std::string name,
                                       const std::filesystem::path &path,
                                       bool recursive = false) noexcept;
  [[nodiscard]] bool
  add_to_collection(std::string_view collection_name,
                    const std::filesystem::path &wall) noexcept;

  [[nodiscard]] std::vector<std::string> list_collection() const noexcept;

  [[nodiscard]] bool merge_collection(std::string_view collection_1,
                                      std::string_view collection_2) noexcept;

  [[nodiscard]] bool remove_collection(std::string_view collection) noexcept;

  [[nodiscard]] bool move_wallpaper(std::string_view origin_collection,
                                    std::string_view dest_collection,
                                    std::string_view wall_name) noexcept;

private:
  std::string_view m_logger;
  wallchanger::cache_lib m_cache;
  wallchanger::path_table m_path_buf;
  std::vector<nlohmann::json> m_previous;
};

} // namespace wallchanger
