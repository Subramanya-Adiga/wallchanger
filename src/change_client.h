#pragma once
#include "message_type.h"
#include "net/client_interface.h"
#include <fmt/chrono.h>
namespace wallchanger {
class change_client : public net::client_interface<MessageType> {
public:
  change_client() { m_active = connect("127.0.0.1", 60000); }

  [[nodiscard]] bool active() const { return m_active; }

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

  void run() {
    while (!m_stop_processing) {
      m_process_server_commands();
    }
  }

private:
  bool m_stop_processing = false;
  bool m_active = false;

  void m_process_server_commands() {
    if (is_connected()) {
      if (!incomming().empty()) {
        auto msg = incomming().pop_front().msg;
        if (msg.validate()) {
          switch (msg.header.id) {
          case MessageType::Server_GetStatus: {
            std::cout << message_helper::msg_to_json(msg) << "\n";
            m_stop_processing = true;
          } break;
          case MessageType::Server_GetPing: {
            auto time_now = std::chrono::system_clock::now();
            auto time_then = msg.time;
            fmt::print("{}\n", std::chrono::duration_cast<std::chrono::seconds>(
                                   time_now - time_then));
            m_stop_processing = true;
          } break;
          case MessageType::Status_Success: {
            m_stop_processing = true;
          } break;
          case MessageType::Client_Accepted:
          case MessageType::Client_AssignID:
          case MessageType::Client_RegisterWithServer:
          case MessageType::Client_UnregisterWithServer:
          case MessageType::Create_Collection:
          case MessageType::Get_Next_Wallpaper:
          case MessageType::Get_Previous_Wallpaper:
          case MessageType::Change_Active_Collection:
          case MessageType::Status_Failure:
            break;
          }
        }
      }
    }
  }
};
} // namespace wallchanger
