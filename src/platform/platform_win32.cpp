#include "platform_win32.h"
#include "windows_helper.h"

wallchanger::platform::win32::background::background() {
  HRESULT res = CoInitialize(nullptr);
  if (FAILED(res)) {
    m_display_err("CoInitialize", res);
    m_initialized = false;
  }
  m_initialized = true;

  res = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(&m_wallpaper));
  m_valid_instance = true;
  if (FAILED(res)) {
    m_display_err("CoCreateInstance", res);
    m_valid_instance = false;
  }
}

wallchanger::platform::win32::background::~background() {
  if (m_initialized) {
    m_wallpaper->Release();
    CoUninitialize();
  }
}

bool wallchanger::platform::win32::background::set_wallpaper(
    std::wstring_view filename, int position) const {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {

    result = m_wallpaper->SetWallpaper(NULL, filename.data());
    if (FAILED(result)) {
      m_display_err("IDesktopWallpaper->SetWallpaper", result);
      return false;
    }

    int pos =
        ((position >= DWPOS_CENTER) && (position <= DWPOS_SPAN)) ? position : 0;

    result =
        m_wallpaper->SetPosition(static_cast<DESKTOP_WALLPAPER_POSITION>(pos));
    if (FAILED(result)) {
      m_display_err("IDestopWallpaper->SetPosition", result);
      return false;
    }
  }
  return (result == S_OK);
}

bool wallchanger::platform::win32::background::set_color(DWORD color) const {
  HRESULT result = S_FALSE;
  if (m_valid_instance) {
    result = m_wallpaper->SetBackgroundColor(color);
    if (FAILED(result)) {
      m_display_err("IDestopWallpaper->SetBackgroundColor", result);
      return false;
    }
  }
  return (result == S_OK);
}

void wallchanger::platform::win32::background::m_display_err(
    std::string_view name, HRESULT res) {
  auto err_text =
      ::win32::to_utf16(::win32::error_handler_win32::fmt_msg(name, res));
  ::MessageBoxW(0, err_text.c_str(), L"Error", MB_ICONERROR | MB_OK);
}