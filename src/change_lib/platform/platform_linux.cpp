#include "platform_linux.h"

namespace wallchanger::platform {

linux::linux_background_handler::linux_background_handler() {
  m_lua_state.open_libraries(sol::lib::base, sol::lib::os);
}
bool linux::linux_background_handler::is_active() { return true; }

bool linux::linux_background_handler::set_background_color(
    unsigned long color) {
  m_lua_state["bg_color"] = color;
  return true;
}

bool linux::linux_background_handler::set_wallpaper(std::string_view wallpaper,
                                                    int position) {
  m_lua_state["background"] = wallpaper;
  m_lua_state["position"] = position;
  auto res =
      m_lua_state.load_file(data_directory() + "/plugins/linux_changer.lua");
  return true;
}

std::string linux::linux_background_handler::get_error() const {
  return m_error;
}

} // namespace wallchanger::platform