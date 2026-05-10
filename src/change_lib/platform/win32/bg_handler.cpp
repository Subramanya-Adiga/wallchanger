#include "platform_win32.hpp"
#include "windows_helper.hpp"

wallchanger::platform::win32::get_image_information::get_image_information(
    const std::string &image) {
  auto status = Gdiplus::GdiplusStartup(&m_start_token, &m_input, nullptr);
  if (SUCCEEDED(status)) {
    m_initialized = true;
    m_image = Gdiplus::Image::FromFile(::win32::to_utf16(image).c_str());
  } else {
    m_error = ::win32::error_handler_win32::fmt_msg("Gdiplus::GdiplusStartup",
                                                    status);
  }
}
// GetFlags,GetPixelFormat
wallchanger::platform::win32::get_image_information::~get_image_information() {
  if (m_initialized) {
    delete m_image;
    Gdiplus::GdiplusShutdown(m_start_token);
  }
}

std::string wallchanger::platform::win32::get_image_information::error() const {
  return m_error;
}

bool wallchanger::platform::win32::get_image_information::initialized() const {
  return m_initialized;
}

std::pair<int, int>
wallchanger::platform::win32::get_image_information::get_dimensions() const {
  return std::make_pair(m_image->GetWidth(), m_image->GetHeight());
}

std::string
wallchanger::platform::win32::get_image_information::retrive_image_properties()
    const {
  UINT buffersize{};
  UINT propcount{};
  m_image->GetPropertySize(&buffersize, &propcount);
  std::vector<Gdiplus::PropertyItem> items(buffersize);
  m_image->GetAllPropertyItems(buffersize, propcount, &items[0]);
  items.resize(propcount);
  std::string ret;
  for (auto &&item : items) {
    ret += fmt::format("{}", item);
  }
  return ret;
}

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