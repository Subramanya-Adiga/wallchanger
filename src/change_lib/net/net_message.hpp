#pragma once
#include "net_common.hpp"
namespace wallchanger::net {

template <typename T> struct message_header {
  T id{};
  uint32_t size{};
  uint32_t hash{};
};

template <typename T> struct message {
  message_header<T> header{};
  std::chrono::time_point<std::chrono::system_clock> time{};
  std::vector<uint8_t> body{};

  void finalize() {
    header.hash = static_cast<uint32_t>(
        wallchanger::helper::crc(body.begin(), body.end()));
    header.size = static_cast<uint32_t>(body.size());
    time = std::chrono::system_clock::now();
  }

  [[nodiscard]] bool validate() {
    auto gen_crc = wallchanger::helper::crc(body.begin(), body.end());
    return gen_crc == header.hash;
  }

  template <typename DataType>
  friend message<T> &operator<<(message<T> &msg, const DataType &data) {
    static_assert(std::is_standard_layout_v<DataType>,
                  "Data is too complex to be pushed into vector");

    size_t body_size = msg.body.size();

    msg.body.resize(msg.body.size() + sizeof(DataType));

    std::memcpy(msg.body.data() + body_size, &data, sizeof(DataType));

    msg.header.size = static_cast<uint32_t>(msg.body.size());

    msg.finalize();

    return msg;
  }

  // Pulls any POD-like data form the message buffer
  template <typename DataType>
  friend message<T> &operator>>(message<T> &msg, DataType &data) {

    static_assert(std::is_standard_layout_v<DataType>,
                  "Data is too complex to be pulled from vector");
    if (msg.validate()) {
      auto body_size = msg.body.size() - sizeof(DataType);

      std::memcpy(&data, msg.body.data() + body_size, sizeof(DataType));

      msg.body.resize(body_size);

      msg.header.size = static_cast<uint32_t>(msg.body.size());
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