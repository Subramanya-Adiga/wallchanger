#pragma once
#include "message_type.h"
#include <net/server_interface.h>
#include <nlohmann/json.hpp>
#include <random>
#include <wall_cache.h>
#include <wall_cache_library.h>

namespace wallchanger {

class change_server : public net::server_interface<MessageType> {
public:
  explicit change_server(uint16_t port)
      : net::server_interface<MessageType>(port), m_cache(true) {
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

    case wallchanger::MessageType::Get_Next_Wallpaper: {
      std::random_device random_device;
      std::mt19937 generator(random_device());
      auto cache = m_cache.get_cache(m_active);
      std::uniform_int_distribution<> dist(1, static_cast<int>(cache.size()));

      bool found = false;
      cache_lib::cache_lib_type::mapped_type ret{};
      int idx{};
      while (!found) {
        idx = dist(generator);
        if (auto state = cache.get_state(idx);
            state == wallchanger::cache_state_e::unused) {
          ret = cache.get(idx);
          cache.set_state(idx, wallchanger::cache_state_e::used);
          found = true;
        }
      }

      uint32_t path_loc = cache.get_loc_id(idx);
      nlohmann::json send;
      send["wallpaper"] = ret;
      send["path"] = m_cache.cache_retrive_path(path_loc);
      send["index"] = idx;
      send["collection"] = m_active;
      m_previous.push_back(send);
      client->send_message(
          message_helper::json_to_msg(MessageType::Get_Next_Wallpaper, send));

      LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
               client->get_id());
    } break;

    case wallchanger::MessageType::Get_Previous_Wallpaper: {
      if (!m_previous.empty()) {
        auto previous = m_previous.back();
        m_previous.pop_back();
        client->send_message(message_helper::json_to_msg(
            MessageType::Get_Previous_Wallpaper, previous));
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
      client->send_message(message_helper::json_to_msg(MessageType::Get_Current,
                                                       m_previous.back()));
      LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
               client->get_id());
    } break;

    case wallchanger::MessageType::Mark_Favorate: {
      auto cache =
          m_cache.get_cache(server_cmd["collection"].get<std::string>());
      cache.set_state(
          server_cmd["index"]
              .get<wallchanger::cache_lib::cache_lib_type::key_type>(),
          wallchanger::cache_state_e::favorate);
      LOG_INFO(
          get_logger_name(),
          "Client:[{}] Marked {} Wallpaper From Collection {} As Favorate\n",
          client->get_id(), server_cmd["collection"].get<std::string>(),
          cache[server_cmd["index"]
                    .get<wallchanger::cache_lib::cache_lib_type::key_type>()]);
      client->send_message(m_success());
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

      wallchanger::cache_lib::cache_lib_type cache;
      auto col_path = server_cmd["col_path"].get<std::string>();
      auto crc_loc = static_cast<uint32_t>(
          wallchanger::helper::crc(col_path.begin(), col_path.end()));
      int counter = 0;
      auto inserter = [&](const std::filesystem::directory_entry &path) {
        if (!path.is_directory()) {
          counter++;
          cache.insert(counter, path.path().filename().string(), crc_loc);
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

    case MessageType::Rename_Collection: {
      m_cache.rename_store(server_cmd["col_name"].get<std::string>(),
                           server_cmd["col_name_new"].get<std::string>());
      client->send_message(m_success());
      LOG_INFO(get_logger_name(), "created renamed:[{}] to:[{}]\n",
               server_cmd["col_name"].get<std::string>(),
               server_cmd["col_name_new"].get<std::string>());
    } break;

    case MessageType::Remove_Collection: {
      m_cache.remove(server_cmd["col_name"].get<std::string>());
      client->send_message(m_success());
      LOG_INFO(get_logger_name(), "removed collection:[{}]\n",
               server_cmd["col_name"].get<std::string>());
    } break;

    case MessageType::List_Collections: {
      nlohmann::json obj;
      obj["list"] = m_cache.cache_list();
      client->send_message(
          message_helper::json_to_msg(MessageType::List_Collections, obj));
      LOG_INFO(get_logger_name(), "client:[{}] requested to list collections\n",
               client->get_id());
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
    MESSAGE_VALIDATE_END
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}

private:
  std::chrono::time_point<std::chrono::system_clock> m_start_time;
  wallchanger::cache_lib m_cache;
  std::vector<nlohmann::json> m_previous;
  std::string m_active;
  inline static net::message<MessageType> m_success() {
    net::message<MessageType> send;
    send.header.id = MessageType::Status_Success;
    send << true;
    return send;
  }
};
} // namespace wallchanger
