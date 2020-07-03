#pragma once
#include "../../pch.h"

namespace wall_changer::platform::linux {

std::string get_desktop_key();

class background{
  DConfClient *m_client;
  bool m_initialized = false;
  bool m_valid_instance = false;

public:
  background();
  ~background();
  bool set_wallpaper(std::string_view filename,
                     std::string_view position) const;
  bool set_color(std::string_view color) const;
};
} // namespace wall_changer::platform::linux
