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

[[maybe_unused]] static uint32_t gen_id() {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<uint32_t> dist;
  return dist(generator);
}

} // namespace wallchanger::helper
