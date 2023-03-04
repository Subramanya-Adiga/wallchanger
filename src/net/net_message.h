#pragma once

namespace wallchanger::net {

template <typename T> struct message_header {
  T id{};
  uint32_t size{};
  uint32_t hash{};
};

template <typename T> struct message {
  message_header<T> header{};
  std::chrono::time_point<std::chrono::system_clock> time{};
  nlohmann::json body{};

  friend message<T> &operator<<(message<T> &msg, const nlohmann::json &data) {
    msg.header.hash = wallchanger::helper::crc(data.begin(), data.end());
    msg.time = std::chrono::system_clock::now();
    msg.header.size = static_cast<uint32_t>(data.size());
    msg.body = data;
    return msg;
  }

  friend message<T> &operator>>(message<T> &msg, nlohmann::json &data) {
    auto gen_crc = wallchanger::helper::crc(msg.body.begin(), msg.body.end());
    if (gen_crc == msg.header.hash) {
      data = msg.body;
    } else {
      throw std::domain_error("Failed CRC Match. Data Maybe Corrupt.");
    }
    return msg;
  }
};

template <typename T> class connection;

template <typename T> struct owned_message {
  std::shared_ptr<connection<T>> remote = nullptr;
  message<T> msg;
};

} // namespace wallchanger::net