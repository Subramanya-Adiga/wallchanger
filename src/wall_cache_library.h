#pragma once
#include "../pch.h"
#include "wall_cache.h"
#include <compare>

namespace wallchanger {

class cache_lib {
  using cache_type = cache<int, std::string>;
  using cache_store = std::pair<std::string, cache_type>;

public:
  using cache_lib_type = cache_type;
  cache_lib() = default;

  template <typename T>
  void insert(const std::string &name, T &&value) noexcept {
    if (!exists(name)) {
      auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
      m_cache_vec.emplace_back(name, std::forward<T>(value));
    }
  }

  [[nodiscard]] cache_lib_type get_cache(std::string_view name) const noexcept;

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
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    return rng_it->second;
  }

  auto operator<=>(const cache_lib &) const = default;

private:
  std::vector<cache_store> m_cache_vec;
  inline void m_clear_empty() noexcept {
    ranges::actions::drop_while(m_cache_vec,
                                [](auto &type) { return type.second.empty(); });
  }
};

} // namespace wallchanger
