#include "platform_win32.h"

wall_changer::platform::win32::background::background() {
  if (HRESULT init = CoInitialize(NULL); init == S_OK) {
    if (HRESULT instance_create =
            CoCreateInstance(__uuidof(DesktopWallpaper), NULL, CLSCTX_ALL,
                             IID_PPV_ARGS(&m_wallpaper));
        instance_create == S_OK) {
      m_valid_instance = true;
    }
    // log
    m_initialized = true;
  }
  // log
}

wall_changer::platform::win32::background::~background() {
  if (m_initialized)
    CoUninitialize();
}

bool wall_changer::platform::win32::background::set_wallpaper(
    std::wstring_view filename, int position) const {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {
    result = m_wallpaper->SetWallpaper(NULL, filename.data());
    if (!(position > 0) || (position > 5)) {
      m_wallpaper->SetPosition(
          static_cast<DESKTOP_WALLPAPER_POSITION>(position));
    } else {
      m_wallpaper->SetPosition(
          static_cast<DESKTOP_WALLPAPER_POSITION>(position));
    }
  }
  return (result == S_OK);
}

bool wall_changer::platform::win32::background::set_color(DWORD color) const {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {
    result = m_wallpaper->SetBackgroundColor(color);
  }
  return (result == S_OK);
}
