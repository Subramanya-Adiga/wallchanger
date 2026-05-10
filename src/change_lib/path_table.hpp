#pragma once
#include "defines.hpp"

namespace wallchanger {
class path_table {
  std::vector<std::pair<u32, std::filesystem::path>> m_store;

public:
  using path_ref = std::reference_wrapper<const std::filesystem::path>;
  path_table();
  void insert(std::filesystem::path path);
  [[nodiscard]] std::optional<path_ref> get(u32 id) const noexcept;
  void store() const noexcept;
  auto operator<=>(const path_table &) const = default;

  [[nodiscard]] bool exists(u32 id) const noexcept;

private:
  bool m_modified = false;
};
} // namespace wallchanger
