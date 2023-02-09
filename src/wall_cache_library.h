#pragma once
#include "wall_cache.h"
#include <compare>

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

  void insert(std::string name, std::string path,
              cache_lib_type value) noexcept;

  [[nodiscard]] cache_lib_type get_cache(std::string_view name) const noexcept;
  [[nodiscard]] std::string_view
  get_cache_path(std::string_view name) const noexcept;
  [[nodiscard]] cache_store get_current() const noexcept { return m_current; }

  [[nodiscard]] bool exists(std::string_view name) const noexcept;
  [[nodiscard]] size_t capacity() const noexcept {
    return m_cache_vec.capacity();
  }
  [[nodiscard]] size_t cache_count() const noexcept {
    return m_cache_vec.size();
  }

  void remove(std::string_view name) noexcept;
  [[nodiscard]] bool empty() const noexcept { return m_cache_vec.empty(); }
  [[nodiscard]] bool modified() const noexcept;

  [[nodiscard]] cache_lib_type &operator[](std::string_view name) noexcept {
    m_clear_empty();
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::name);
    m_current = *rng_it;
    return m_current.cache;
  }

  auto operator<=>(const cache_lib &) const = default;

  void serialize() const;
  void deserialize();

private:
  inline void m_clear_empty() noexcept {
    if (!empty()) {
      ranges::actions::drop_while(
          m_cache_vec, [](auto &type) { return type.cache.empty(); });
    }
  }
};

} // namespace wallchanger
