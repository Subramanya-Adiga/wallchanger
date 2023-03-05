#pragma once

namespace wallchanger {
enum class MessageType : uint32_t {
  Server_GetStatus,
  Server_GetPing,
  Client_Accepted,
  Client_AssignID,
  Client_RegisterWithServer,
  Client_UnregisterWithServer,
};
}