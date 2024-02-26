#include "platform_linux.h"

namespace wallchanger::platform {

bool linux::linux_background_handler::is_active() { return true; }

bool linux::linux_background_handler::set_background_color(
    unsigned long color) {
  return true;
}

bool linux::linux_background_handler::set_wallpaper(std::string_view wallpaper,
                                                    int position) {
  return true;
}

std::string linux::linux_background_handler::get_error() const {
  return m_error;
}

} // namespace wallchanger::platform