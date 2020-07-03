#include "platform_linux.h"

wall_changer::platform::linux::background::background() {
  if (m_client = dconf_client_new(); m_client != nullptr) {
    m_initialized = true;
  } else {
    // log error
  }
}

wall_changer::platform::linux::background::~background() {
  if (m_initialized)
    g_object_unref(G_OBJECT(m_client));
}

bool wall_changer::platform::linux::background::set_wallpaper(
    std::string_view filename, std::string_view position) const {

  bool result = false;

  return result;
}
