#pragma once
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#define MESSAGE_VALIDATE_BEGIN(x) if (x) {
#define MESSAGE_VALIDATE_END                                                   \
  }                                                                            \
  else {                                                                       \
    LOG_ERR(get_logger_name(), "Message Corrupted\n");                         \
  }

namespace wallchanger::helper {

static constexpr int s_port_number = 60000;

consteval std::array<std::uint_fast32_t, 256> generate_crc_table() noexcept {
  constexpr auto reverse_polynomial = std::uint_fast32_t{0xEDB88320UL};
  struct byte_checksum {
    constexpr std::uint_fast32_t operator()() noexcept {
      auto checksum = static_cast<std::uint_fast32_t>(n++);

      for (auto i = 0; i < 8; ++i) {
        checksum =
            (checksum >> 1) ^ ((checksum & 0x1u) ? reverse_polynomial : 0);
      }

      return checksum;
    }

  private:
    unsigned n = 0;
  };

  std::array<std::uint_fast32_t, 256> table{};
  std::generate(table.begin(), table.end(), byte_checksum{});

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
               return table[(checksum ^ value) & 0xFFU] ^ (checksum >> 8);
             });
}

[[maybe_unused]] static uint32_t gen_id() {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<uint32_t> dist;
  return dist(generator);
}

} // namespace wallchanger::helper
