#pragma once
#include "../../pch.h"

namespace wall_changer::platform::win32 {
class background {
  IDesktopWallpaper *m_wallpaper = nullptr;
  bool m_initialized = false;
  bool m_valid_instance = false;

public:
  background();
  ~background();
  bool is_active() const { return (m_initialized && m_valid_instance); }
  bool set_wallpaper(std::wstring_view filename, int position) const;
  bool set_color(DWORD color) const;
};
} // namespace wall_changer::platform::win32
