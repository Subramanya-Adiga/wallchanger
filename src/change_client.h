#pragma once
#include "message_type.h"
#include "net/client_interface.h"

namespace wallchanger {
class change_client : public net::client_interface<MessageType> {
public:
  void ping_server() {
    net::message<MessageType> msg;
    msg.header.id = MessageType::Server_GetPing;
    msg << true;
    send_message(msg);
  }

  void get_server_status() {
    net::message<MessageType> msg;
    msg.header.id = MessageType::Server_GetStatus;
    msg << true;
    send_message(msg);
  }
};
} // namespace wallchanger
