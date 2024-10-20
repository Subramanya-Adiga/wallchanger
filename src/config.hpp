#pragma once

#include "log.hpp"
#include "scoped_timer.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <outcome.hpp>

namespace outcome = OUTCOME_V2_NAMESPACE;

inline std::string log_directory() {
#ifdef _WIN32
  std::string ret = std::getenv("userprofile");
  return ret + R"(\AppData\Local\Wallchanger\logs\)";
#endif
#ifdef __linux__
  std::string ret = std::getenv("HOME");
  return ret + R"(/.local/share/wallchanger/logs/)";
#endif
}

inline std::string data_directory() {
#ifdef _WIN32
  std::string ret = std::getenv("userprofile");
  return ret + R"(\AppData\Local\Wallchanger\)";
#endif
#ifdef __linux__
  std::string ret = std::getenv("HOME");
  return ret + R"(/.config/wallchanger/)";
#endif
}