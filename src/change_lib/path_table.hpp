#pragma once

namespace wallchanger {
class path_table {
  std::vector<std::pair<uint32_t, std::filesystem::path>> m_store;

public:
  using path_ref = std::reference_wrapper<const std::filesystem::path>;
  path_table();
  void insert(std::filesystem::path path);
  [[nodiscard]] std::optional<path_ref> get(uint32_t id) const noexcept;
  void store() const noexcept;
  auto operator<=>(const path_table &) const = default;

  [[nodiscard]] bool exists(uint32_t id) const noexcept;

private:
  bool m_modified = false;
};
} // namespace wallchanger