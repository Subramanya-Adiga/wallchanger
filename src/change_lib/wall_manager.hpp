#pragma once
#include "path_table.hpp"
#include "wall_cache_library.hpp"
#include <nlohmann/json_fwd.hpp>

namespace wallchanger {

class manager {
public:
  explicit manager(std::string_view logger_name);
  void store();
  void mark_favorate();
  nlohmann::json next_wall();
  nlohmann::json previous_wall();

private:
  std::string_view m_logger;
  wallchanger::cache_lib m_cache;
  wallchanger::path_table m_path_buf;
  std::vector<nlohmann::json> m_previous;
};

} // namespace wallchanger
