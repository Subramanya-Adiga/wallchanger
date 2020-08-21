#pragma once
#include "../pch.h"
#include <compare>
#include <concepts>

namespace wallchanger {

enum class cache_state_e { unused = 0, used = 1, favorate = 11 };

template <typename Key, typename Value> struct cache_type_struct {
  Key cache_key;
  Value cache_value;
  cache_state_e cache_state;
  cache_type_struct(Key cache_key1, Value cache_value1, cache_state_e state1)
      : cache_key(cache_key1), cache_value(cache_value1), cache_state(state1) {}
};

template <typename Key, typename Value> class cache {
  using cache_t = typename std::pair<Key, std::pair<Value, int>>;

public:
  using key_type = Key;
  using mapped_type = Value;
  using value_type = typename std::vector<cache_t>::value_type;
  using size_type = typename std::vector<cache_t>::size_type;
  using reference = typename std::vector<cache_t>::reference;
  using const_reference = typename std::vector<cache_t>::const_reference;
  using iterator = typename std::vector<cache_t>::iterator;
  using const_iterator = typename std::vector<cache_t>::const_iterator;

  enum state { s_null = 0, s_unused = 1, s_used = 2 };

  cache() = default;
  explicit cache(size_type size) {
    size_type size_ = 0;
    size_ = (size == 0) ? std::numeric_limits<char>::max() : size;
    m_cache_vec.reserve(size_);
  }
  void reserve(size_type size) { m_cache_vec.reserve(size); }

  template <typename val>
  void insert(key_type key, val &&value) requires std::same_as<val, Value> {
    if (!exists(key)) {
      m_cache_vec.emplace_back(
          std::move(key), std::make_pair(std::forward<val>(value), s_unused));
      m_modified = true;
    }
  }

  template <typename val>
  void replace(key_type key, val &&value) noexcept
      requires std::same_as<val, Value> {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::first);
    if (it_rng->first == key) {
      it_rng->second.first = std::forward<val>(value);
      it_rng->second.second = s_unused;
      m_modified = true;
    }
  }

  void clear() noexcept {
    m_cache_vec.clear();
    m_modified = true;
  }
  void erase(key_type key) noexcept {
    ranges::actions::drop_while(
        m_cache_vec, [&](auto &type) { return (key == type.first); });
    m_modified = true;
  }
  void shrink_to_fit() { m_cache_vec.shrink_to_fit(); }

  [[nodiscard]] size_type size() const noexcept { return m_cache_vec.size(); }
  [[nodiscard]] size_type capacity() const noexcept {
    return m_cache_vec.capacity();
  }
  [[nodiscard]] bool empty() const noexcept { return m_cache_vec.empty(); }
  [[nodiscard]] bool modified() const noexcept { return m_modified; }
  [[nodiscard]] bool exists(key_type key) const noexcept {
    if (!empty()) {
      auto rng_it = ranges::find(m_cache_vec, key, &value_type::first);
      return (rng_it->first == key);
    }
    return false;
  }

  [[nodiscard]] mapped_type get(key_type key) const noexcept {
    return at_(key);
  }
  [[nodiscard]] mapped_type get(key_type key) noexcept { return at_(key); }
  mapped_type &operator[](key_type key) noexcept { return at_(key); }
  mapped_type &operator[](key_type key) const noexcept { return at_(key); }

  void set_state(key_type key, state new_state) noexcept {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::first);
    if (it_rng->first == key) {
      it_rng->second.second = new_state;
      m_modified = true;
    }
  }
  [[nodiscard]] state get_state(key_type key) const noexcept {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::first);
    return static_cast<state>(it_rng->second.second);
  }

  reference front() noexcept { return m_cache_vec.front(); }
  reference back() noexcept { return m_cache_vec.back(); }
  [[nodiscard]] const_reference front() const noexcept {
    return m_cache_vec.front();
  }
  [[nodiscard]] const_reference back() const noexcept {
    return m_cache_vec.back();
  }

  iterator begin() noexcept { return std::begin(m_cache_vec); }
  iterator end() noexcept { return std::end(m_cache_vec); }
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return std::cbegin(m_cache_vec);
  }
  [[nodiscard]] const_iterator cend() const noexcept {
    return std::cend(m_cache_vec);
  }

  bool operator!() const noexcept { return this->m_cache_vec.empty(); }
  auto operator<=>(const cache &) const = default;

private:
  bool m_modified = false;
  std::vector<value_type> m_cache_vec;
  [[nodiscard]] mapped_type at_(key_type key) const noexcept {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::first);
    return it_rng->second.first;
  }
};

} // namespace wallchanger
