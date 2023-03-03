#pragma once

namespace wallchanger::net {

template <typename T> struct message_header {
  T id{};
  uint32_t size{};
  uint32_t hash{};
};

template <typename T> struct message {
  message_header<T> header{};
  nlohmann::json body{};
};

template <typename T> class connection;

template <typename T> struct owned_message {
  std::shared_ptr<connection<T>> remote = nullptr;
  message<T> msg;
};

} // namespace wallchanger::net