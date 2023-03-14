#pragma once
#include "../wall_background.h"
#include <Shobjidl.h>
namespace wallchanger::platform::win32 {

class win32_background_handler : public wallchanger::background_handler {
  IDesktopWallpaper *m_wallpaper{};
  bool m_initialized{};
  bool m_valid_instance{};

public:
  win32_background_handler();
  ~win32_background_handler() override;
  [[nodiscard]] bool is_active() override;
  [[nodiscard]] bool set_wallpaper(std::string_view wallpaper,
                                   int position) override;
  [[nodiscard]] bool set_background_color(unsigned long color) override;
};

} // namespace wallchanger::platform::win32
