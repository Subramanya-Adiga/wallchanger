#pragma once
#include "../wall_background.h"

namespace wallchanger::platform::linux {

class linux_background_handler : public wallchanger::background_handler {
public:
  linux_background_handler() = default;
  ~linux_background_handler() override = default;
  [[nodiscard]] bool is_active() override;
  [[nodiscard]] bool set_wallpaper(std::string_view wallpaper,
                                   int position) override;
  [[nodiscard]] bool set_background_color(unsigned long color) override;
  [[nodiscard]] std::string get_error() const;
};

} // namespace wallchanger::platform::linux
