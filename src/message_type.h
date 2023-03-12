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
  Create_Collection,
  Get_Next_Wallpaper,
  Get_Previous_Wallpaper,
  Change_Active_Collection,
  Status_Success,
  Status_Failure,
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