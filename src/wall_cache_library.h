#pragma once
#include "../pch.h"
#include "wall_cache.h"

namespace wall_changer {

class wall_cache_lib {
  using cache_type = wall_cache<int, std::string>;
  using cache_store = std::pair<std::string, cache_type>;
  using small_cache = std::vector<cache_store>;

public:
  using value_type = cache_type;

  wall_cache_lib() = default;

  template <typename T>
  void insert(const std::string &name, T &&value) noexcept {
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    if (rng_it->first != name) {
      m_cache_vec.emplace_back(name, std::forward<T>(value));
    }
  }

  [[nodiscard]] value_type get(std::string_view name) const noexcept;
  void erase(std::string_view name) noexcept;
  [[nodiscard]] bool exists(std::string_view name) const noexcept;
  [[nodiscard]] size_t cache_count() const noexcept {
    return m_cache_vec.size();
  }
  [[nodiscard]] bool modified() const noexcept;

  [[nodiscard]] value_type &operator[](std::string_view name) noexcept {
    m_clear_empty();
    auto rng_it = ranges::find(m_cache_vec, name, &cache_store::first);
    return rng_it->second;
  }

private:
  small_cache m_cache_vec;
  inline void m_clear_empty() noexcept {
    ranges::actions::drop_while(
        m_cache_vec, [](auto &type) { return type.second.is_empty(); });
  }
};

} // namespace wall_changer
