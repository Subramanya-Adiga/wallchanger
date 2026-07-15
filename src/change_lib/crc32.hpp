#pragma once

namespace wallchanger::helper {

consteval std::array<std::uint_fast32_t, 256> generate_crc_table() noexcept {
  constexpr auto reverse_polynomial = std::uint_fast32_t{0xEDB88320UL};
  struct byte_checksum {
    constexpr std::uint_fast32_t operator()() noexcept {
      auto checksum = static_cast<std::uint_fast32_t>(n++);

      for (auto i = 0; i < 8; ++i) {
        checksum = (checksum >> 1) ^
                   (((checksum & 0x1U) != 0U) ? reverse_polynomial : 0);
      }

      return checksum;
    }

  private:
    unsigned n = 0;
  };

  std::array<std::uint_fast32_t, 256> table{};
  std::ranges::generate(table, byte_checksum{});

  return table;
}

template <typename InputIterator>
constexpr std::uint_fast32_t crc(InputIterator first, InputIterator last) {
  auto table = generate_crc_table();

  return std::uint_fast32_t{0xFFFFFFFFUL} &
         ~std::accumulate(
             first, last,
             ~std::uint_fast32_t{0} & std::uint_fast32_t{0xFFFFFFFFUL},
             [&](std::uint_fast32_t checksum, std::uint_fast8_t value) {
               return table.at((checksum ^ value) & 0xFFU) ^ (checksum >> 8);
             });
}
} // namespace wallchanger::helper
