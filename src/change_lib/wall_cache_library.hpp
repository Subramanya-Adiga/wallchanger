#pragma once
#include "wall_cache.hpp"

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

  [[nodiscard]] outcome::result<void> insert(std::string name,
                                             cache_lib_type value) noexcept;

  [[nodiscard]] std::optional<cache_lib_cref>
  get_cache(std::string_view name) const noexcept;

  [[nodiscard]] std::optional<cache_lib_ref>
  get_cache(std::string_view name) noexcept;

  [[nodiscard]] std::optional<cache_lib_cref> get_current() const noexcept;

  [[nodiscard]] outcome::result<void>
  change_active(std::string_view new_active) noexcept;
  [[nodiscard]] outcome::result<void>
  rename_store(std::string_view from_name, std::string_view to_name) noexcept;

  [[nodiscard]] outcome::result<void>
  merge_cache(std::string_view col1, std::string_view col2) noexcept;

  [[nodiscard]] outcome::result<void>
  move_cache_item(std::string_view source, std::string_view dest,
                  std::string_view item_name) noexcept;

  [[nodiscard]] bool exists(std::string_view name) const noexcept;
  [[nodiscard]] size_t cache_count() const noexcept;

  [[nodiscard]] outcome::result<void> remove(std::string_view name) noexcept;
  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool modified() const noexcept;

  [[nodiscard]] std::vector<std::string> cache_list() const noexcept;

  auto operator<=>(const cache_lib &) const = default;

  void serialize() const;
  bool deserialize();

private:
  std::string m_active_name;
  void m_clear_empty() noexcept {
    if (!is_empty()) {
      auto res = std::ranges::remove_if(
          m_cache_vec, [](auto &&data) { return data.second.empty(); });
    }
  }
};

} // namespace wallchanger
