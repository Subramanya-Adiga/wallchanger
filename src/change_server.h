#pragma once
#include "message_type.h"
#include "net/server_interface.h"
#include "wall_cache.h"
#include <nlohmann/json.hpp>

#define MESSAGE_VALIDATE_BEGIN(x) if (x) {
#define MESSAGE_VALIDATE_END                                                   \
  }                                                                            \
  else {                                                                       \
    LOG_ERR(get_logger_name(), "Message Corrupted\n");                         \
  }

namespace wallchanger {

class change_server : public net::server_interface<MessageType> {
public:
  explicit change_server(uint16_t port)
      : net::server_interface<MessageType>(port) {
    m_start_time = std::chrono::system_clock::now();
  }

  void store_state() { m_cache.serialize(); }

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

    MESSAGE_VALIDATE_BEGIN(msg.validate())

    auto server_cmd = message_helper::msg_to_json(msg);
    switch (msg.header.id) {

    case wallchanger::MessageType::Server_GetStatus: {
      LOG_INFO(get_logger_name(), "[{}]:Requested Server Status\n",
               client->get_id());
      nlohmann::json ret;
      ret["connections"] = m_connections.size();
      auto time_now = std::chrono::system_clock::now();
      ret["uptime"] = std::chrono::time_point<std::chrono::system_clock>(
                          time_now - m_start_time)
                          .time_since_epoch()
                          .count();
      client->send_message(
          message_helper::json_to_msg(MessageType::Server_GetStatus, ret));
    } break;

    case wallchanger::MessageType::Server_GetPing: {
      LOG_INFO(get_logger_name(), "[{}]: Server Ping\n", client->get_id());
      msg << true;
      client->send_message(msg);
    } break;

    case wallchanger::MessageType::Change_Active_Collection: {
      m_active.resize(msg.body.size());
      std::memcpy(m_active.data(), msg.body.data(), msg.body.size());
      LOG_INFO(get_logger_name(),
               "Client:[{}] changed active collection to:{} \n",
               client->get_id(), m_active);

      client->send_message(m_success());
    } break;

    case wallchanger::MessageType::Create_Collection: {

      wallchanger::cache_lib::cache_lib_type cache;
      auto col_path = server_cmd["col_path"].get<std::string>();

      int counter = 0;
      auto inserter = [&](const std::filesystem::directory_entry &path) {
        if (!path.is_directory()) {
          counter++;
          cache.insert(counter, path.path().filename().string());
        }
      };

      if (!server_cmd["recursive"].get<bool>()) {
        ranges::for_each(std::filesystem::directory_iterator(col_path),
                         inserter);
      } else {
        ranges::for_each(
            std::filesystem::recursive_directory_iterator(col_path), inserter);
      }

      m_cache.insert(server_cmd["new_col_name"].get<std::string>(), col_path,
                     cache);
      LOG_INFO(get_logger_name(), "created collection:[{}] path:[{}]\n",
               server_cmd["new_col_name"].get<std::string>(),
               server_cmd["col_path"].get<std::string>());

      client->send_message(m_success());
    } break;

    case wallchanger::MessageType::Get_Next_Wallpaper: {
      LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
               client->get_id());
    } break;

    case wallchanger::MessageType::Get_Previous_Wallpaper: {
      LOG_INFO(get_logger_name(), "Client:[{}] Requested Previous Wallpaper\n",
               client->get_id());
    } break;
    case MessageType::Client_Accepted:
    case MessageType::Client_AssignID:
    case MessageType::Client_RegisterWithServer:
    case MessageType::Client_UnregisterWithServer:
    case MessageType::Status_Success:
    case MessageType::Status_Failure:
      break;
    }
    MESSAGE_VALIDATE_END
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}

private:
  std::chrono::time_point<std::chrono::system_clock> m_start_time;
  wallchanger::cache_lib m_cache;
  std::string m_active;
  inline static net::message<MessageType> m_success() {
    net::message<MessageType> send;
    send.header.id = MessageType::Status_Success;
    send << true;
    return send;
  }
};
} // namespace wallchanger