#pragma once
#include "net/net_message.h"

namespace wallchanger {
enum class MessageType : uint32_t {
  Server_GetStatus,
  Server_GetPing,
  Client_Accepted,
  Client_AssignID,
  Client_RegisterWithServer,
  Client_UnregisterWithServer,
  // Genarel Messages
  Get_Next_Wallpaper,
  Get_Previous_Wallpaper,
  Mark_Favorate,
  Get_Current,
  // Collection Messages
  Create_Collection,
  Change_Active_Collection,
  Rename_Collection,
  Remove_Collection,
  List_Collections,
  // Status Messages
  Status_Success,
  Status_Failure,
};

enum class server_error_code : uint32_t {
  Message_Validation_Error,
  Client_Connect_Error,
  Client_Validation_Error
};

struct server_error {
  server_error_code code;
  std::string what;
};

struct message_helper {
  static nlohmann::json msg_to_json(const net::message<MessageType> &msg) {
    std::vector<uint8_t> res;
    res.resize(msg.body.size());
    std::memcpy(res.data(), msg.body.data(), msg.body.size());
    return nlohmann::json::from_cbor(res);
  }

  static net::message<MessageType> json_to_msg(MessageType type,
                                               nlohmann::json &json) {
    wallchanger::net::message<wallchanger::MessageType> msg;
    msg.header.id = type;
    auto vec = nlohmann::json::to_cbor(json);
    msg.body.resize(vec.size());
    std::memcpy(msg.body.data(), vec.data(), vec.size());
    msg.finalize();
    return msg;
  }
};

} // namespace wallchanger

NLOHMANN_JSON_SERIALIZE_ENUM(
    wallchanger::server_error_code,
    {
        {wallchanger::server_error_code::Client_Connect_Error,
         "client_connect_error"},
        {wallchanger::server_error_code::Client_Validation_Error,
         "client_validation_error"},
        {wallchanger::server_error_code::Message_Validation_Error,
         "message_validation_error"},
    });

namespace nlohmann {
template <> struct adl_serializer<wallchanger::server_error> {
  static void to_json(json &j, const wallchanger::server_error &rhs) {
    j = nlohmann::json{{"code", rhs.code}, {"error", rhs.what}};
  }
  static void from_json(const json &j, wallchanger::server_error &rhs) {
    if (!j.is_null()) {
      rhs.code = j.at("code").get<wallchanger::server_error_code>();
      rhs.what = j.at("error").get<std::string>();
    }
  }
};
} // namespace nlohmann