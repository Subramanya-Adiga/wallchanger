#pragma once
#include "defines.hpp"
#include <format>

namespace wallchanger {

enum class cache_state_e : u8 {
  null,
  unused,
  used,
  favorate,
};

template <typename Value> struct cache_item {
  Value cache_value;
  cache_state_e cache_state{};
  u32 loc{};
  cache_item() = default;

  explicit cache_item(Value cache_value1, cache_state_e state1, u32 loc1)
      : cache_value(std::forward<Value>(cache_value1)), cache_state(state1),
        loc(loc1) {}
  auto operator<=>(const cache_item &) const = default;
};

template <typename Value> class cache {
  using cache_t = cache_item<Value>; // std::pair<Key,
                                     // std::pair<Value, int>>;

public:
  using value_type = typename std::vector<cache_t>::value_type;
  using size_type = typename std::vector<cache_t>::size_type;
  using pointer = typename std::vector<cache_t>::pointer;
  using const_pointer = typename std::vector<cache_t>::const_pointer;
  using reference = typename std::vector<cache_t>::reference;
  using const_reference = typename std::vector<cache_t>::const_reference;
  using iterator = typename std::vector<cache_t>::iterator;
  using const_iterator = typename std::vector<cache_t>::const_iterator;
  using reverse_iterator = typename std::vector<cache_t>::reverse_iterator;
  using const_reverse_iterator =
      typename std::vector<cache_t>::const_reverse_iterator;

  cache() = default;
  explicit cache(size_type size) noexcept
      : m_list(std::vector<cache_t>(size)) {}

  [[nodiscard]] bool contains(const Value &val) const noexcept {
    auto rng_it = std::ranges::find(m_list, val, &cache_t::cache_value);
    return (rng_it != std::ranges::end(m_list));
  }

  template <typename val>
  void insert(val &&value, u32 loc)
    requires std::same_as<val, Value>
  {
    if (!contains(value)) {
      m_list.emplace_back(std::forward<val>(value), cache_state_e::unused, loc);
      m_modified = true;
    }
  }

  void insert_elem(cache_t &&item) {
    if (auto item_itr = std::ranges::find(m_list, item);
        item_itr == std::ranges::end(m_list)) {
      m_list.emplace_back(std::move(item));
      m_modified = true;
    }
  }

  void clear() noexcept {
    m_list.clear();
    m_modified = true;
  }

  iterator erase(const_iterator pos) noexcept {
    m_modified = true;
    return m_list.erase(pos);
  }

  [[nodiscard]] size_type size() const noexcept { return m_list.size(); }
  [[nodiscard]] bool empty() const noexcept { return m_list.empty(); }
  [[nodiscard]] bool modified() const noexcept { return m_modified; }

  [[nodiscard]] pointer data() noexcept {
    m_modified = true;
    return m_list.data();
  }
  [[nodiscard]] const_pointer data() const noexcept { return m_list.data(); }
  
  // Iterators
  [[nodiscard]] iterator begin() noexcept {
    m_modified = true;
    return std::begin(m_list);
  }
  [[nodiscard]] iterator end() noexcept {
    m_modified = true;
    return std::end(m_list);
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return std::cbegin(m_list);
  }
  [[nodiscard]] const_iterator end() const noexcept {
    return std::cend(m_list);
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    return std::cbegin(m_list);
  }
  [[nodiscard]] const_iterator cend() const noexcept {
    return std::cend(m_list);
  }

  [[nodiscard]] reverse_iterator rbegin() noexcept {
    m_modified = true;
    return std::rbegin(m_list);
  }
  [[nodiscard]] reverse_iterator rend() noexcept {
    m_modified = true;
    return std::rend(m_list);
  }

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return rbegin();
  }
  [[nodiscard]] const_reverse_iterator rend() const noexcept { return rend(); }

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }
  [[nodiscard]] const_reverse_iterator crend() const noexcept { return rend(); }

  // Operators
  reference operator[](size_type idx) const { return m_list.at(idx); }

  reference operator[](size_type idx) { return m_list.at(idx); }

  auto operator<=>(const cache &) const = default;

private:
  bool m_modified = false;
  std::vector<value_type> m_list;
};
} // namespace wallchanger

template <>
struct std::formatter<wallchanger::cache_state_e>
    : std::formatter<std::string_view> {
  auto format(wallchanger::cache_state_e state,
              std::format_context &ctx) const {
    std::string_view name = "unknown";
    switch (state) {
    case wallchanger::cache_state_e::null:
      name = "null";
      break;
    case wallchanger::cache_state_e::unused:
      name = "unused";
      break;
    case wallchanger::cache_state_e::used:
      name = "used";
      break;
    case wallchanger::cache_state_e::favorate:
      name = "favorate";
      break;
    }
    return std::formatter<std::string_view>::format(name, ctx);
  }
};
template <>
struct std::formatter<wallchanger::cache_item<std::string>>
    : std::formatter<string_view> {
  auto format(const wallchanger::cache_item<std::string> &obj,
              std::format_context &ctx) const {
    auto out = std::format("Value:{}\nState:{}\nLocID:{:X}\n", obj.cache_value,
                           obj.cache_state, obj.loc);
    return std::formatter<string_view>::format(out, ctx);
  }
};
