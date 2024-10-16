#pragma once
#include "change_state.hpp"
#include "message_type.hpp"
#include <fstream>
#include <json_helper.hpp>
#include <net/server_interface.hpp>
#include <nlohmann/json.hpp>
#include <path_table.hpp>
#include <random>
#include <wall_cache.hpp>
#include <wall_cache_library.hpp>
#include <wall_error.hpp>

namespace wallchanger {

class change_server : public net::server_interface<MessageType> {
public:
  explicit change_server(uint16_t port)
      : net::server_interface<MessageType>(port), m_cache(true),
        m_state(get_logger_name()) {
    m_start_time = std::chrono::system_clock::now();
  }

  void store_state() { m_state.store_state(); }

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
    m_process(msg.header.id, client, msg);
    MESSAGE_VALIDATE_END
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}

private:
  std::chrono::time_point<std::chrono::system_clock> m_start_time;

  wallchanger::state m_state;

  wallchanger::cache_lib m_cache;
  path_table m_path_buf;

  std::vector<nlohmann::json> m_previous;
  std::string m_active;

  inline static net::message<MessageType> m_success() {
    net::message<MessageType> send;
    send.header.id = MessageType::Status_Success;
    send << true;
    return send;
  }

  void m_process(
      wallchanger::MessageType type,
      std::shared_ptr<wallchanger::net::connection<wallchanger::MessageType>>
          &client,
      wallchanger::net::message<wallchanger::MessageType> &msg) {
    auto server_cmd = message_helper::msg_to_json(msg);
    switch (type) {

    case wallchanger::MessageType::Get_Next_Wallpaper: {
      auto send = m_state.get_next_wallpaper();
      client->send_message(
          message_helper::json_to_msg(MessageType::Get_Next_Wallpaper, send));

      LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
               client->get_id());
    } break;

    case wallchanger::MessageType::Get_Previous_Wallpaper: {
      auto prev = m_state.get_previous_wallpaper();
      if (!prev.empty()) {
        client->send_message(message_helper::json_to_msg(
            MessageType::Get_Previous_Wallpaper, prev));
        LOG_INFO(get_logger_name(),
                 "Client:[{}] Requested Previous Wallpaper\n",
                 client->get_id());
      } else {
        net::message<MessageType> msg_failure;
        msg_failure.header.id = MessageType::Status_Failure;
        msg_failure << false;
        client->send_message(msg_failure);
        LOG_ERR(get_logger_name(),
                "Client:[{}] Requested Previous Wallpaper.\n No Previous "
                "Wallpaper To Provide.\n",
                client->get_id());
      }
    } break;

    case wallchanger::MessageType::Get_Current: {
      auto cur = m_state.get_next_wallpaper();
      client->send_message(
          message_helper::json_to_msg(MessageType::Get_Current, cur));
      LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
               client->get_id());
    } break;

    case wallchanger::MessageType::Mark_Favorate: {
      // auto cache =
      //     m_cache.get_cache(server_cmd["collection"].get<std::string>())
      //         .value()
      //         .get();
      // cache.set_state(
      //     server_cmd["index"]
      //         .get<wallchanger::cache_lib::cache_lib_type::key_type>(),
      //     wallchanger::cache_state_e::favorate);
      // LOG_INFO(
      //     get_logger_name(),
      //     "Client:[{}] Marked {} Wallpaper From Collection {} As Favorate\n",
      //     client->get_id(), server_cmd["collection"].get<std::string>(),
      //     cache[server_cmd["index"]
      //               .get<wallchanger::cache_lib::cache_lib_type::key_type>()]);
      // client->send_message(m_success());
    } break;

      // Collection Messages
    case wallchanger::MessageType::Change_Active_Collection: {
      m_active.resize(msg.body.size());
      std::memcpy(m_active.data(), msg.body.data(), msg.body.size());
      m_cache.change_active(m_active);
      LOG_INFO(get_logger_name(),
               "Client:[{}] changed active collection to:{} \n",
               client->get_id(), m_active);

      client->send_message(m_success());
    } break;

    case wallchanger::MessageType::Create_Collection: {
      m_state.create_collection(server_cmd);
      LOG_INFO(get_logger_name(), "created collection:[{}]\n",
               server_cmd["new_col_name"].get<std::string>());
      client->send_message(m_success());
    } break;

    case wallchanger::MessageType::Add_To_Collection: {
      if (m_state.add_to_collection(server_cmd)) {
        LOG_INFO(get_logger_name(), "added wall:[{}] to collection:[{}]\n",
                 server_cmd["col_name"].get<std::string>(),
                 server_cmd["wall"].get<std::string>());
        client->send_message(m_success());
      }
    } break;

    case MessageType::Rename_Collection: {
      m_cache.rename_store(server_cmd["col_name"].get<std::string>(),
                           server_cmd["col_name_new"].get<std::string>());
      client->send_message(m_success());
      LOG_INFO(get_logger_name(), "created renamed:[{}] to:[{}]\n",
               server_cmd["col_name"].get<std::string>(),
               server_cmd["col_name_new"].get<std::string>());
    } break;

    case MessageType::Remove: {
      if (server_cmd["wall_only"].get<bool>()) {
        if (auto dat =
                m_cache.get_cache(server_cmd["col"].get<std::string_view>())) {

          client->send_message(m_success());
          LOG_INFO(get_logger_name(),
                   "removed wallpaper:[{}] from collection:[{}]\n",
                   server_cmd["wall"].get<std::string_view>(),
                   server_cmd["col"].get<std::string_view>());
        }
      } else {
        m_cache.remove(server_cmd["col"].get<std::string_view>());
        client->send_message(m_success());
        LOG_INFO(get_logger_name(), "removed collection:[{}]\n",
                 server_cmd["col"].get<std::string_view>());
      }
    } break;

    case MessageType::List_Collections: {
      nlohmann::json obj;
      if (!server_cmd["col_only"].get<bool>()) {
        if (auto dat =
                m_cache.get_cache(server_cmd["col"].get<std::string_view>())) {
          obj["list"] = dat.value().get();
          obj["list-only"] = false;
          client->send_message(
              message_helper::json_to_msg(MessageType::List_Collections, obj));
          LOG_INFO(get_logger_name(),
                   "client:[{}] requested to list collections\n",
                   client->get_id());
        }
      } else {
        obj["list"] = m_cache.cache_list();
        obj["list-only"] = true;
        client->send_message(
            message_helper::json_to_msg(MessageType::List_Collections, obj));
        LOG_INFO(get_logger_name(),
                 "client:[{}] requested to list collections\n",
                 client->get_id());
      }
    } break;

    case MessageType::Merge_Collection: {
      if (m_state.merge_collection(server_cmd, client->get_id())) {
        client->send_message(m_success());
      }
    } break;

    case MessageType::Move: {

      if (m_state.move_collectoion(server_cmd, client->get_id())) {
        client->send_message(m_success());
      }

    } break;

      // Server Status Messages
    case wallchanger::MessageType::Server_GetStatus: {
      LOG_INFO(get_logger_name(), "[{}]:Requested Server Status\n",
               client->get_id());
      nlohmann::json ret;
      ret["connections"] = m_connections.size();

      auto time_now = std::chrono::system_clock::now();

      ret["uptime"] = time_now.time_since_epoch().count() -
                      m_start_time.time_since_epoch().count();
      client->send_message(
          message_helper::json_to_msg(MessageType::Server_GetStatus, ret));
    } break;

    case wallchanger::MessageType::Server_GetPing: {
      LOG_INFO(get_logger_name(), "[{}]: Server Ping\n", client->get_id());
      msg << msg.time;
      client->send_message(msg);
    } break;

    case MessageType::Client_Accepted:
    case MessageType::Client_AssignID:
    case MessageType::Client_RegisterWithServer:
    case MessageType::Client_UnregisterWithServer:
    case MessageType::Status_Success:
    case MessageType::Status_Failure:

      break;
    }
  }
};
} // namespace wallchanger
