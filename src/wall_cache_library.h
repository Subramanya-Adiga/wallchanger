#pragma once
#include "../pch.h"
#include "wall_cache.h"

namespace wall_changer {

class wall_cache_lib {
  using cache_type = wall_cache<int, std::string>;
  using cache_store = std::pair<std::string, cache_type>;
  using growth_50_option_t =
      boost::container::small_vector_options<boost::container::growth_factor<
          boost::container::growth_factor_50>>::type;
  using small_cache =
      boost::container::small_vector<cache_store, 4, void, growth_50_option_t>;

public:
  using value_type = cache_type;

  wall_cache_lib() = default;
  explicit wall_cache_lib(std::string_view cache_loc)
      : m_cache_loc(cache_loc), m_inserted(false) {}

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
  std::string_view m_cache_loc;
  bool m_inserted;
  bool m_loaded;
  small_cache m_cache_vec;
  inline void m_clear_empty() noexcept {
    ranges::actions::drop_while(
        m_cache_vec, [](auto &type) { return type.second.is_empty(); });
  }
};

} // namespace wall_changer
