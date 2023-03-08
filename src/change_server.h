#pragma once
#include "message_type.h"
#include "net/server_interface.h"
#include <nlohmann/json.hpp>

namespace wallchanger {
class change_server : public net::server_interface<MessageType> {
public:
  explicit change_server(uint16_t port)
      : net::server_interface<MessageType>(port) {
    m_start_time = std::chrono::system_clock::now();
  }

protected:
  bool on_client_connect(
      std::shared_ptr<wallchanger::net::connection<wallchanger::MessageType>>
          client) override {
    wallchanger::net::message<wallchanger::MessageType> msg;
    msg.header.id = wallchanger::MessageType::Client_Accepted;
    msg << true;
    client->send_message(msg);
    return true;
  }

  void on_client_disconnect(
      std::shared_ptr<net::connection<MessageType>> client) override {
    LOG_INFO(get_logger_name(), "Client:[{}] Removed", client->get_id());
  }

  void on_message(
      std::shared_ptr<wallchanger::net::connection<wallchanger::MessageType>>
          client,
      wallchanger::net::message<wallchanger::MessageType> &msg) override {

    switch (msg.header.id) {

    case wallchanger::MessageType::Server_GetStatus: {
      LOG_INFO(get_logger_name(), "[{}]:Requested Server Status\n",
               client->get_id());
      msg.body.clear();
      nlohmann::json ret;
      ret["connections"] = m_connections.size();
      auto time_now = std::chrono::system_clock::now();
      ret["uptime"] = std::chrono::time_point<std::chrono::system_clock>(
                          time_now - m_start_time)
                          .time_since_epoch()
                          .count();
      msg << ret;
      client->send_message(msg);
      break;
    }

    case wallchanger::MessageType::Server_GetPing: {
      LOG_INFO(get_logger_name(), "[{}]: Server Ping\n", client->get_id());
      msg << true;
      client->send_message(msg);
      break;
    }

    default:
      break;
    }
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}

private:
  std::chrono::time_point<std::chrono::system_clock> m_start_time;
};
} // namespace wallchanger