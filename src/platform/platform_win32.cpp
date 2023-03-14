#include "platform_win32.h"
#include "windows_helper.h"

wallchanger::platform::win32::win32_background_handler::
    win32_background_handler() {
  HRESULT res = CoInitialize(nullptr);
  if (FAILED(res)) {
    m_error = ::win32::error_handler_win32::fmt_msg("CoInitialize", res);
    m_initialized = false;
  }
  m_initialized = true;

  res = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(&m_wallpaper));
  m_valid_instance = true;
  if (FAILED(res)) {
    m_error = ::win32::error_handler_win32::fmt_msg("CoCreateInstance", res);
    m_valid_instance = false;
  }
}

wallchanger::platform::win32::win32_background_handler::
    ~win32_background_handler() {
  if (m_initialized) {
    m_wallpaper->Release();
    CoUninitialize();
  }
}

bool wallchanger::platform::win32::win32_background_handler::is_active() {
  return (m_initialized && m_valid_instance);
}

bool wallchanger::platform::win32::win32_background_handler::set_wallpaper(
    std::string_view wallpaper, int position) {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {

    result = m_wallpaper->SetWallpaper(
        nullptr, ::win32::to_utf16(wallpaper.data()).c_str());
    if (FAILED(result)) {
      m_error = ::win32::error_handler_win32::fmt_msg(
          "IDesktopWallpaper->SetWallpaper", result);
      return false;
    }

    int pos =
        ((position >= DWPOS_CENTER) && (position <= DWPOS_SPAN)) ? position : 0;

    result =
        m_wallpaper->SetPosition(static_cast<DESKTOP_WALLPAPER_POSITION>(pos));
    if (FAILED(result)) {
      m_error = ::win32::error_handler_win32::fmt_msg(
          "IDestopWallpaper->SetPosition", result);
      return false;
    }
  }
  return (result == S_OK);
}

bool wallchanger::platform::win32::win32_background_handler::
    set_background_color(unsigned long color) {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {
    result = m_wallpaper->SetBackgroundColor(color);
    if (FAILED(result)) {
      m_error = ::win32::error_handler_win32::fmt_msg(
          "IDestopWallpaper->SetBackgroundColor", result);
      return false;
    }
  }
  return (result == S_OK);
}