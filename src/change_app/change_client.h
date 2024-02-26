#pragma once
#include "message_type.h"
// #include "platform/platform_win32.h" Used To Get Information About Wallpaper
// Image
#include "wall_background.h"
#include <fmt/chrono.h>
#include <net/client_interface.h>

namespace wallchanger {
class change_client : public net::client_interface<MessageType> {
public:
  change_client() { m_active = connect("127.0.0.1", helper::s_port_number); }

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
    while (is_connected() && (m_processing)) {
      m_process_server_commands();
    }
  }

private:
  bool m_processing = true;
  bool m_active = false;

  void m_process_server_commands() {
    bool msg_processed = false;
    if (!incomming().empty()) {
      auto msg = incomming().pop_front().msg;
      MESSAGE_VALIDATE_BEGIN(msg.validate())
      switch (msg.header.id) {
      case MessageType::Server_GetStatus: {
        auto status_msg = message_helper::msg_to_json(msg);
        fmt::print("Connections:{} Uptime:{} \n",
                   status_msg["connections"].get<size_t>(),
                   status_msg["uptime"].get<uint64_t>());
        msg_processed = true;
      } break;
      case MessageType::Server_GetPing: {
        auto time_now = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point time_then;
        msg >> time_then;
        fmt::print("{}\n",
                   std::chrono::duration<double>(time_now - time_then).count());
        msg_processed = true;
      } break;
      case MessageType::Status_Success: {
        msg_processed = true;
      } break;
      case MessageType::Status_Failure: {
        LOG_ERR(get_logger_name(), "Server Error\n");
        msg_processed = true;
      } break;
      case MessageType::Get_Next_Wallpaper: {
        auto get = message_helper::msg_to_json(msg);
        auto background = background_handler::create();
        if (background) {
          if (background->is_active()) {
            if (!background->set_wallpaper(
                    get["path"].get<std::string>() + "/" +
                        get["wallpaper"].get<std::string>(),
                    2)) {
              LOG_ERR(get_logger_name(), "{}\n", background->get_error());
            }
          } else {
            LOG_ERR(get_logger_name(), "{}\n", background->get_error());
          }
        } else {
          LOG_ERR(get_logger_name(), "{}\n", background->get_error());
        }
        msg_processed = true;
      } break;
      case MessageType::Get_Previous_Wallpaper: {
        auto get = message_helper::msg_to_json(msg);
        auto background = background_handler::create();
        if (background) {
          if (background->is_active()) {
            if (!background->set_wallpaper(
                    get["path"].get<std::string>() + "/" +
                        get["wallpaper"].get<std::string>(),
                    2)) {
              LOG_ERR(get_logger_name(), "{}\n", background->get_error());
            }
          } else {
            LOG_ERR(get_logger_name(), "{}\n", background->get_error());
          }
        } else {
          LOG_ERR(get_logger_name(), "{}\n", background->get_error());
        }
        msg_processed = true;
      } break;
      case MessageType::Get_Current: {
        auto obj = message_helper::msg_to_json(msg);
        // wallchanger::platform::win32::get_image_information info(
        //     obj["path"].get<std::string>() + "/" +
        //     obj["wallpaper"].get<std::string>());
        msg_processed = true;
      } break;
      case MessageType::List_Collections: {
        auto obj = message_helper::msg_to_json(msg);
        fmt::print("{}\n",
                   fmt::join(obj["list"].get<std::vector<std::string>>(), ","));
        msg_processed = true;
      } break;
      case MessageType::Create_Collection:
      case MessageType::Change_Active_Collection:
      case MessageType::Mark_Favorate:
      case MessageType::Rename_Collection:
      case MessageType::Remove_Collection:
      case MessageType::Client_Accepted:
      case MessageType::Client_AssignID:
      case MessageType::Client_RegisterWithServer:
      case MessageType::Client_UnregisterWithServer:
        break;
      }
      MESSAGE_VALIDATE_END
      m_processing = !msg_processed;
    }
  }
};
} // namespace wallchanger
