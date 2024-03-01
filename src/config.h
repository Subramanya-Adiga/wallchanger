#pragma once

#include "log.h"
#include "scoped_timer.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

inline std::string log_directory() {
#ifdef _WINDOWS
  std::string ret = std::getenv("userprofile");
  return ret + R"(\AppData\Local\Wallchanger\logs\)";
#endif
#ifdef __linux__
  std::string ret = std::getenv("HOME");
  return ret + R"(/.local/share/wallchanger/logs/)";
#endif
}

inline std::string data_directory() {
#ifdef _WINDOWS
  std::string ret = std::getenv("userprofile");
  return ret + R"(\AppData\Local\Wallchanger\data\)";
#endif
#ifdef __linux__
  std::string ret = std::getenv("HOME");
  return ret + R"(/.config/wallchanger/data/)";
#endif
}