#pragma once
#include "wall_cache.h"

namespace wallchanger {
template <typename T> struct cache_store_t {
  std::string name;
  std::string path;
  T cache;
  cache_store_t() = default;
  explicit cache_store_t(std::string cache_name, std::string cache_path,
                         T content)
      : name(std::move(cache_name)), path(std::move(cache_path)),
        cache(std::forward<T>(content)) {}
  auto operator<=>(const cache_store_t &) const = default;
};

class cache_lib {
  using cache_type = cache<int, std::string>;
  using cache_store = cache_store_t<cache_type>;
  std::vector<cache_store> m_cache_vec;
  mutable cache_store m_current;

public:
  using cache_lib_type = cache_type;
  cache_lib() = default;
  explicit cache_lib(bool load);

  void insert(std::string name, std::string path,
              cache_lib_type value) noexcept;

  [[nodiscard]] cache_lib_type get_cache(std::string_view name) const noexcept;
  [[nodiscard]] std::string_view
  get_cache_path(std::string_view name) const noexcept;
  [[nodiscard]] cache_store get_current() const noexcept;
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

  // [[nodiscard]] cache_lib_type &operator[](std::string_view name) noexcept {
  //   m_clear_empty();
  //   if (exists(name)) {
  //     auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
  //     return rng_it->cache;
  //   }
  //   return m_cache_vec.at(0).cache;
  // }

  auto operator<=>(const cache_lib &) const = default;

  void serialize() const;
  bool deserialize();

private:
  std::string m_active_name;
  inline void m_clear_empty() noexcept {
    if (!is_empty()) {
      ranges::actions::drop_while(
          m_cache_vec, [](auto &type) { return type.cache.empty(); });
    }
  }
};

} // namespace wallchanger
