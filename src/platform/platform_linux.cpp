#include "platform_linux.h"

bool wall_changer::platform::linux::background::set_wallpaper(
    std::string_view filename, std::string_view position) const {
  return false;
}
