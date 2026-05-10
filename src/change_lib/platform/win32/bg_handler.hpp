#pragma once
#include "../wall_background.hpp"
#include <Shobjidl.h>
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

static std::string format_asci(const Gdiplus::PropertyItem &item) {
  std::string ret;
  ret.resize(item.length);
  std::memcpy(&ret[0], item.value, item.length);
  return ret;
}

template <typename T>
static std::vector<T> format_array(const Gdiplus::PropertyItem &item) {
  std::vector<T> ret(item.length);
  std::memcpy(&ret[0], item.value, item.length);
  return ret;
}

template <typename T> static T format_data(const Gdiplus::PropertyItem &item) {
  T *data = std::bit_cast<T *>(item.value);
  return *data;
}

template <>
struct fmt::formatter<Gdiplus::PropertyItem> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(Gdiplus::PropertyItem g, FormatContext &ctx) {
    std::string out;
    switch (g.id) {
    case PropertyTagArtist:
      out += fmt::format("Artist:{}\n", format_asci(g));
      break;
    case PropertyTagDateTime:
      out += fmt ::format("DateTime:{}\n", format_asci(g));
      break;
    case PropertyTagSoftwareUsed:
      out += fmt ::format("SoftwareUsed:{}\n", format_asci(g));
      break;
    case PropertyTagEquipMake:
      out += fmt ::format("EquipMaker:{}\n", format_asci(g));
      break;
    case PropertyTagEquipModel:
      out += fmt ::format("EquipModel:{}\n", format_asci(g));
      break;
    default:
      break;
    }
    return formatter<string_view>::format(out, ctx);
  }
};

namespace wallchanger::platform::win32 {

class get_image_information {
  Gdiplus::GdiplusStartupInput m_input;
  ULONG_PTR m_start_token{};
  bool m_initialized{};
  std::string m_error;
  Gdiplus::Image *m_image = nullptr;

public:
  explicit get_image_information(const std::string &image);
  ~get_image_information();
  [[nodiscard]] std::string error() const;
  [[nodiscard]] bool initialized() const;

  [[nodiscard]] std::pair<int, int> get_dimensions() const;
  [[nodiscard]] std::string retrive_image_properties() const;
};

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
