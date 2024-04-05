#pragma once
namespace wallchanger {

enum class cache_state_e : uint32_t {
  null,
  unused,
  used,
  favorate,
};

template <typename Value> struct cache_item {
  Value cache_value;
  cache_state_e cache_state{};
  uint32_t loc{};
  cache_item() = default;

  explicit cache_item(Value cache_value1, cache_state_e state1, uint32_t loc1)
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
    auto rng_it = ranges::find(m_list, val, &cache_t::cache_value);
    return (rng_it != ranges::end(m_list));
  }

  template <typename val>
  void insert(val &&value, uint32_t loc)
    requires std::same_as<val, Value>
  {
    if (!contains(value)) {
      m_list.emplace_back(std::forward<val>(value), cache_state_e::unused, loc);
      m_modified = true;
    }
  }

  void clear() noexcept {
    m_list.clear();
    m_modified = true;
  }

  [[nodiscard]] size_type size() const noexcept { return m_list.size(); }
  [[nodiscard]] bool empty() const noexcept { return m_list.empty(); }
  [[nodiscard]] bool modified() const noexcept { return m_modified; }

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
struct fmt::formatter<wallchanger::cache_state_e>
    : fmt::formatter<std::string_view> {
  template <typename FormatContext>
  auto fomrmat(wallchanger::cache_state_e obj, FormatContext &ctx) const {
    std::string_view out = "NULL";
    switch (obj) {
    case wallchanger::cache_state_e::null:
      out = "NULL";
      break;
    case wallchanger::cache_state_e::unused:
      out = "Unused";
      break;
    case wallchanger::cache_state_e::used:
      out = "Used";
      break;
    case wallchanger::cache_state_e::favorate:
      out = "Favorate";
      break;
    }
    return formatter<std::string_view>::format(out, ctx);
  }
};

template <>
struct fmt::formatter<wallchanger::cache_item<std::string>>
    : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const wallchanger::cache_item<std::string> &obj,
              FormatContext &ctx) const {
    std::string out =
        fmt::format("Value:{}\n,State:{}\n,LocID:{:X}\n", obj.cache_value,
                    fmt::underlying(obj.cache_state), obj.loc);
    return formatter<string_view>::format(out, ctx);
  }
};
