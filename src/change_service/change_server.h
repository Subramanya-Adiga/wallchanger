#pragma once
#include "message_type.h"
#include <fstream>
#include <json_helper.h>
#include <net/server_interface.h>
#include <nlohmann/json.hpp>
#include <path_table.h>
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

  void store_state() {
    if (!m_previous.empty()) {
      std::ofstream hist(data_directory() + "/data/history.json",
                         std::ios::out);
      nlohmann::json obj;
      obj["histoy"] = m_previous;
      hist << std::setw(4) << obj << "\n";
    }
    m_path_buf.store();
    m_cache.serialize();
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

    MESSAGE_VALIDATE_BEGIN(msg.validate())

    auto server_cmd = message_helper::msg_to_json(msg);
    switch (msg.header.id) {

    case wallchanger::MessageType::Get_Next_Wallpaper: {
      std::random_device random_device;
      std::mt19937 generator(random_device());
      if (auto dat = m_cache.get_cache(m_active)) {
        auto cache = dat.value().get();
        std::uniform_int_distribution<> dist(1, static_cast<int>(cache.size()));

        bool found = false;
        cache_lib::cache_lib_type::value_type ret{};
        size_t idx{};
        while (!found) {
          idx = static_cast<size_t>(dist(generator));
          if (auto state = cache[idx].cache_state;
              state == wallchanger::cache_state_e::unused) {
            ret = cache[idx];
            cache[idx].cache_state = wallchanger::cache_state_e::used;
            found = true;
          }
        }

        uint32_t path_loc = cache[idx].loc;
        nlohmann::json send;
        send["wallpaper"] = ret.cache_value;
        send["path"] = m_path_buf.get(path_loc).value().get();
        send["index"] = idx;
        send["collection"] = m_active;
        m_previous.push_back(send);
        client->send_message(
            message_helper::json_to_msg(MessageType::Get_Next_Wallpaper, send));

        LOG_INFO(get_logger_name(), "Client:[{}] Requested Next Wallpaper\n",
                 client->get_id());
      }
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

      wallchanger::cache_lib::cache_lib_type cache;
      auto col_path = server_cmd["col_path"].get<std::string>();
      auto crc_loc = static_cast<uint32_t>(
          wallchanger::helper::crc(col_path.begin(), col_path.end()));
      int counter = 0;
      auto inserter = [&](const std::filesystem::directory_entry &path) {
        if (!path.is_directory()) {
          counter++;
          cache.insert(path.path().filename().string(), crc_loc);
        }
      };

      if (!server_cmd["recursive"].get<bool>()) {
        ranges::for_each(std::filesystem::directory_iterator(col_path),
                         inserter);
      } else {
        ranges::for_each(
            std::filesystem::recursive_directory_iterator(col_path), inserter);
      }

      m_path_buf.insert(col_path);
      m_cache.insert(server_cmd["new_col_name"].get<std::string>(), cache);
      LOG_INFO(get_logger_name(), "created collection:[{}] path:[{}]\n",
               server_cmd["new_col_name"].get<std::string>(),
               server_cmd["col_path"].get<std::string>());

      client->send_message(m_success());
    } break;

    case wallchanger::MessageType::Add_To_Collection: {
      auto col_name = server_cmd["col_name"].get<std::string>();
      auto wall = server_cmd["wall"].get<std::filesystem::path>();
      auto wall_path = wall.parent_path().string();
      auto path_crc = static_cast<uint32_t>(
          wallchanger::helper::crc(wall_path.begin(), wall_path.end()));
      if (auto dat = m_cache.get_cache(col_name)) {
        auto cache = dat.value().get();

        cache.insert(wall.filename().string(), path_crc);
        m_path_buf.insert(wall_path);

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
      auto col1 = server_cmd["col1"].get<std::string_view>();
      auto col2 = server_cmd["col2"].get<std::string_view>();
      m_cache.merge_cache(col1, col2);
      client->send_message(m_success());
      LOG_INFO(get_logger_name(),
               "client:[{}] requested to merge collections {} {}\n",
               client->get_id(), col1, col2);
    } break;

    case MessageType::Move: {
      auto col_frm = server_cmd["col_cur"].get<std::string_view>();
      auto col_to = server_cmd["col_new"].get<std::string_view>();
      auto wall = server_cmd["wall"].get<std::string_view>();

      if (auto cache_frm_opt = m_cache.get_cache(col_frm)) {
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
    MESSAGE_VALIDATE_END
  }

  void on_client_validated(
      std::shared_ptr<net::connection<MessageType>> client) override {}

private:
  std::chrono::time_point<std::chrono::system_clock> m_start_time;
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
};
} // namespace wallchanger
