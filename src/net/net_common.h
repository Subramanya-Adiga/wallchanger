#pragma once
#define ASIO_STANDALONE
#include "../log.h"
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace wallchanger::helper {

consteval std::array<std::uint32_t, 256> generate_crc_table() noexcept {
  constexpr auto reverse_polynomial = std::uint32_t{0xEDB88320UL};
  struct byte_checksum {
    constexpr std::uint32_t operator()() noexcept {
      auto checksum = static_cast<std::uint32_t>(n++);

      for (auto i = 0; i < 8; ++i) {
        checksum =
            (checksum >> 1) ^ ((checksum & 0x1u) ? reverse_polynomial : 0);
      }

      return checksum;
    }

  private:
    unsigned n = 0;
  };

  std::array<std::uint32_t, 256> table{};
  std::generate(table.begin(), table.end(), byte_checksum{});

  return table;
}

template <typename InputIterator>
constexpr std::uint32_t crc(InputIterator first, InputIterator last) {
  auto table = generate_crc_table();

  return std::uint32_t{0xFFFFFFFFUL} &
         ~std::accumulate(
             first, last, ~std::uint32_t{0} & std::uint32_t{0xFFFFFFFFUL},
             [&](std::uint32_t checksum, std::uint8_t value) {
               return table[(checksum ^ value) & 0xFFU] ^ (checksum >> 8);
             });
}
} // namespace wallchanger::helper