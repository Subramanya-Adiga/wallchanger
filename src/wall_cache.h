#pragma once
#include "../pch.h"
#include <compare>
#include <concepts>
/**
 * @file wall_cache.h
 * Implementaion Of Key Value Based Flat Map Type Of Container With
 * State(Used,Unused,Null).
 */

namespace wallchanger {

/// Cache State
enum cache_state_e { null = -1, unused = 0, used = 1, favorate = 11 };

/// Representation Of Cache Item
template <typename Key, typename Value> struct cache_type_struct {
  Key cache_key;                 ///< Cache Key
  Value cache_value;             ///< Cache Value
  cache_state_e cache_state;     ///< Cache State
  cache_type_struct() = default; ///< Default Constructor
  /**
   * @brief Explicit Constructor
   * @param cache_key1
   * @param cache_value1
   * @param state1
   */
  explicit cache_type_struct(Key cache_key1, Value cache_value1,
                             cache_state_e state1)
      : cache_key(std::move(cache_key1)),
        cache_value(std::forward<Value>(cache_value1)), cache_state(state1) {}
  /// Threeway Comparator Operator
  auto operator<=>(const cache_type_struct &) const = default;
};

/**
 * Falt Map Type Container For Cache Items
 */
template <typename Key, typename Value> class cache {
  using cache_t = cache_type_struct<Key, Value>; // std::pair<Key,
                                                 // std::pair<Value, int>>;

public:
  using key_type = Key;      ///<  KeyType
  using mapped_type = Value; ///< Mapped Type
  using value_type = typename std::vector<cache_t>::value_type;
  using size_type = typename std::vector<cache_t>::size_type;
  using reference = typename std::vector<cache_t>::reference;
  using const_reference = typename std::vector<cache_t>::const_reference;
  using iterator = typename std::vector<cache_t>::iterator;
  using const_iterator = typename std::vector<cache_t>::const_iterator;

  enum state { s_null = 0, s_unused = 1, s_used = 2 };

  cache() = default; ///< Default Constructor
  /**
   * @brief Constructor With Size
   * @details Construct Container For Size Items
   * @param size
   */
  explicit cache(size_type size) {
    size_type size_ = 0;
    size_ = (size == 0) ? std::numeric_limits<char>::max() : size;
    m_cache_vec.reserve(size_);
  }
  void reserve(size_type size) { m_cache_vec.reserve(size); }
  /**
   *  @brief Insertion
   *  @details Insert Value Of Val with Assosiated Key Into Container
   */
  template <typename val>
  void insert(key_type key, val &&value) requires std::same_as<val, Value> {
    if (!exists(key)) {
      m_cache_vec.emplace_back(std::move(key), std::move(value),
                               cache_state_e::unused);
      m_modified = true;
    }
  }

  template <typename val>
  void replace(key_type key, val &&value) noexcept
      requires std::same_as<val, Value> {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::cache_key);
    if (it_rng->cache_key == key) {
      it_rng->cache_value = std::forward<val>(value);
      it_rng->cache_state = cache_state_e::unused;
      m_modified = true;
    }
  }

  /// Clears Contents Of Container
  void clear() noexcept {
    m_cache_vec.clear();
    m_modified = true;
  }
  void erase(key_type key) noexcept {
    ranges::actions::drop_while(
        m_cache_vec, [&](auto &type) { return (key == type.cache_key); });
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
      auto rng_it = ranges::find(m_cache_vec, key, &value_type::cache_key);
      return (rng_it->cache_key == key);
    }
    return false;
  }

  [[nodiscard]] mapped_type get(key_type key) const noexcept {
    return at_(key);
  }
  [[nodiscard]] mapped_type get(key_type key) noexcept { return at_(key); }
  mapped_type &operator[](key_type key) noexcept { return at_(key); }
  mapped_type &operator[](key_type key) const noexcept { return at_(key); }

  void set_state(key_type key, cache_state_e new_state) noexcept {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::cache_key);
    if (it_rng->cache_key == key) {
      it_rng->cache_state = new_state;
      m_modified = true;
    }
  }
  [[nodiscard]] cache_state_e get_state(key_type key) const noexcept {
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::cache_key);
    return static_cast<cache_state_e>(it_rng->cache_state);
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
    auto it_rng = ranges::find(m_cache_vec, key, &value_type::cache_key);
    return it_rng->cache_value;
  }
};

} // namespace wallchanger
