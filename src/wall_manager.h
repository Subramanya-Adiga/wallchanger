#pragma once
#include "../pch.h"
#include "wall_cache_library.h"

namespace wallchanger {

class manager {
public:
  manager() = default;
  // Cache And Cache Library Releated Functions
  cache_lib::cache_lib_type build_cache(std::string_view path,
                                        std::string_view cache_name);
  void create_cache_lib(std::string_view name);
  void add_to_cache_lib(std::string_view name,
                        cache_lib::cache_lib_type content);

  cache_lib get_lib_from_collection(std::string_view name) const;

  // Favoriate Releated Function
  void add_to_favoriate();

  // changing Releated Functions
  void next();
  void previous();
  void set_from_favoriate();
  std::string current() const { return m_current_wallpaper; }

private:
  cache_lib::cache_lib_type m_favoriate;
  std::vector<std::pair<std::string, cache_lib>> m_collection;
  std::pair<std::string, cache_lib> m_current;
  std::string m_current_wallpaper;
};
} // namespace wallchanger
