#pragma once
#include "message_type.h"
#include "net/client_interface.h"
#include "platform/platform_win32.h"
#include "wall_background.h"
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
          case MessageType::Status_Failure: {
            LOG_ERR(get_logger_name(), "Server Error\n");
            m_stop_processing = true;
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
            m_stop_processing = true;
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
            m_stop_processing = true;
          } break;
          case MessageType::Get_Current: {
            auto obj = message_helper::msg_to_json(msg);
            wallchanger::platform::win32::get_image_information info(
                obj["path"].get<std::string>() + "/" +
                obj["wallpaper"].get<std::string>());
            m_stop_processing = true;
          } break;
          case MessageType::List_Collections: {
            auto obj = message_helper::msg_to_json(msg);
            fmt::print(
                "{}\n",
                fmt::join(obj["list"].get<std::vector<std::string>>(), ","));
            m_stop_processing = true;
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
        }
      }
    }
  }
};
} // namespace wallchanger
