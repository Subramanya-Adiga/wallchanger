#pragma once
#include "path_table.h"
#include "wall_cache.h"

namespace wallchanger {
class cache_lib {
  using cache_type = cache<std::string>;
  using cache_store = std::pair<std::string, cache_type>;
  std::vector<cache_store> m_cache_vec;
  cache_store m_current;

public:
  using cache_lib_type = cache_type;
  using cache_lib_ref = std::reference_wrapper<cache_lib_type>;
  using cache_lib_cref = std::reference_wrapper<const cache_lib_type>;

  cache_lib() = default;
  explicit cache_lib(bool load);

  void insert(std::string name, std::string path,
              cache_lib_type value) noexcept;

  [[nodiscard]] std::optional<cache_lib_cref>
  get_cache(std::string_view name) const noexcept;

  [[nodiscard]] std::optional<cache_lib_ref>
  get_cache(std::string_view name) noexcept;

  [[nodiscard]] std::string_view
  get_cache_path(std::string_view name) const noexcept;

  [[nodiscard]] std::string cache_retrive_path(uint32_t id) const noexcept;
  void cache_push_path(std::filesystem::path path) noexcept;

  [[nodiscard]] std::optional<cache_lib_cref> get_current() const noexcept;
  [[nodiscard]] std::string get_current_name() const noexcept;

  void change_active(std::string_view new_active) noexcept;
  void rename_store(std::string_view from_name,
                    std::string_view to_name) noexcept;

  [[nodiscard]] bool exists(std::string_view name) const noexcept;
  [[nodiscard]] size_t capacity() const noexcept;
  [[nodiscard]] size_t cache_count() const noexcept;

  void remove(std::string_view name) noexcept;
  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool modified() const noexcept;

  [[nodiscard]] std::vector<std::string> cache_list() const noexcept;

  auto operator<=>(const cache_lib &) const = default;

  void serialize() const;
  bool deserialize();

private:
  std::string m_active_name;
  path_table m_table;
  inline void m_clear_empty() noexcept {
    if (!is_empty()) {
      ranges::actions::drop_while(
          m_cache_vec, [](auto &type) { return type.second.empty(); });
    }
  }
};

} // namespace wallchanger
