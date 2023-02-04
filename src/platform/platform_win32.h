#pragma once
#include <Shobjidl.h>
namespace wallchanger::platform::win32 {

class background {
  IDesktopWallpaper *m_wallpaper = nullptr;
  bool m_initialized;
  bool m_valid_instance;

public:
  background();
  ~background();
  background(const background &) = delete;
  background(background &&) = delete;
  background &operator=(const background &) = delete;
  background &operator=(background &&) = delete;

  [[nodiscard]] bool is_active() const {
    return (m_initialized && m_valid_instance);
  }
  [[nodiscard]] bool set_wallpaper(std::wstring_view filename,
                                   int position) const;
  [[nodiscard]] bool set_color(DWORD color) const;

private:
  static void m_display_err(std::string_view name, HRESULT res);
};

} // namespace wallchanger::platform::win32
