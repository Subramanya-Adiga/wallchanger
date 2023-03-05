#pragma once
#include "message_type.h"
#include "net/server_interface.h"

namespace wallchanger {
class change_server : public net::server_interface<MessageType> {
public:
  explicit change_server(uint16_t port)
      : net::server_interface<MessageType>(port) {}

  bool on_client_connect(
      std::shared_ptr<net::connection<MessageType>> client) override {
    return true;
  }

  void on_client_disconnect(
      std::shared_ptr<net::connection<MessageType>> client) override {
    LOG_INFO("server_logger", "Client:[{}] Removed", client->get_id());
  }

  void on_message(std::shared_ptr<net::connection<MessageType>> client,
                  net::message<MessageType> &msg) override {
    switch (msg.header.id) {
    case MessageType::Server_GetStatus:
      break;
    case MessageType::Server_GetPing: {
      LOG_INFO("server_logger", "[{}] Server Ping\n", client->get_id());
      client->send_message(msg);
      break;
    }
    case MessageType::Client_Accepted:
      break;
    case MessageType::Client_AssignID:
      break;
    case MessageType::Client_RegisterWithServer:
      break;
    case MessageType::Client_UnregisterWithServer:
      break;
    }
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}
};
} // namespace wallchanger