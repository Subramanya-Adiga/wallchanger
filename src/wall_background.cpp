#include "wall_background.h"

#ifdef _WINDOWS
#include "platform/platform_win32.h"
#elif __linux__
#include "platform/platform_linux.h"
#endif

std::string wallchanger::background_handler::get_error() const {
  return m_error;
}

std::unique_ptr<wallchanger::background_handler>
wallchanger::background_handler::create() {
#ifdef _WINDOWS
  return std::make_unique<
      wallchanger::platform::win32::win32_background_handler>();
#elif __linux__
  return std::make_unique<
      wallchanger::platform::linux::linux_background_handler>();
#endif
}