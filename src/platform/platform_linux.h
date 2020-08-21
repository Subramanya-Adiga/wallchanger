#pragma once
#include "../../pch.h"

namespace wallchanger::platform::linux {

class background {
  bool m_initialized = false;
  bool m_valid_instance = false;

public:
  [[nodiscard]] bool set_wallpaper(std::string_view filename,
                                   std::string_view position) const;
  [[nodiscard]] bool set_color(std::string_view color) const;
};
} // namespace wallchanger::platform::linux
