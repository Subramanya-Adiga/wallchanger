#pragma once

#include "json_helper.h"
#include "log.h"
#include "net/net_common.h"
#include "scoped_timer.h"

#ifdef _WINDOWS
#include "platform/windows_helper.h"
#include <windows.h>
#endif

inline std::string log_directory() {
#ifdef _WINDOWS
  std::string ret = std::getenv("userprofile");
  return ret + R"(\AppData\Local\Wallchanger\logs\)";
#endif
}