#pragma once
#include "../../pch.h"

namespace wall_changer::platform::win32 {

struct error_handler_win32 {
  error_handler_win32() = default;
  error_handler_win32(error_handler_win32 &&) = delete;
  error_handler_win32(const error_handler_win32 &) = delete;
  error_handler_win32 &operator=(error_handler_win32 &&) = delete;
  error_handler_win32 &operator=(const error_handler_win32 &) = delete;
  ~error_handler_win32() {
    if (m_msg_buf != NULL)
      LocalFree(m_msg_buf);
  }
  std::string fmt_msg(std::string_view func_name) {
    auto id = GetLastError();
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&m_msg_buf, 0, NULL);
    return fmt::format("{} failed with error {}: {}", func_name, id,
                       (LPTSTR)m_msg_buf);
  }

private:
  LPVOID m_msg_buf = NULL;
};

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
