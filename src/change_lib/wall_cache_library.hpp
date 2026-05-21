#pragma once
#include "wall_cache.hpp"
#include <expected>
#include <system_error>

namespace wallchanger {

class cache_lib {
  using cache_type = cache<std::string>;
  using cache_store = std::pair<std::string, cache_type>;
  std::vector<cache_store> m_cache_vec;
  u32 m_current = {};

public:
  using cache_lib_type = cache_type;
  using slice = std::span<cache_lib_type::value_type>;
  using const_slice = std::span<const cache_lib_type::value_type>;

  cache_lib() = default;
  explicit cache_lib(bool load);

  [[nodiscard]] std::string_view active_cache_name() const noexcept;

  [[nodiscard]] slice get_current() noexcept;
  [[nodiscard]] const_slice get_current() const noexcept;

  [[nodiscard]] std::expected<bool, std::error_code>
  insert(std::string name, cache_lib_type value) noexcept;

  [[nodiscard]] std::expected<const_slice, std::error_code>
  get_cache(std::string_view name) const noexcept;

  [[nodiscard]] std::expected<slice, std::error_code>
  get_cache(std::string_view name) noexcept;

  [[nodiscard]] std::expected<bool, std::error_code>
  change_active(std::string_view new_active) noexcept;
  [[nodiscard]] std::expected<bool, std::error_code>
  rename_store(std::string_view from_name, std::string_view to_name) noexcept;

  [[nodiscard]] std::expected<bool, std::error_code>
  merge_cache(std::string_view col1, std::string_view col2) noexcept;

  [[nodiscard]] std::expected<bool, std::error_code>
  move_cache_item(std::string_view source, std::string_view dest,
                  std::string_view item_name) noexcept;

  [[nodiscard]] bool exists(std::string_view name) const noexcept;
  [[nodiscard]] size_t cache_count() const noexcept;

  [[nodiscard]] std::expected<bool, std::error_code>
  remove(std::string_view name) noexcept;
  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool modified() const noexcept;

  [[nodiscard]] std::vector<std::string> cache_list() const noexcept;

  auto operator<=>(const cache_lib &) const = default;
  const cache_lib_type &operator[](std::string_view name) const noexcept;
  cache_lib_type &operator[](std::string_view name) noexcept;

  void serialize() const;
  bool deserialize();

private:
  std::string m_active_name;
};

} // namespace wallchanger
