#pragma once
namespace wallchanger {
class background_handler {
public:
  virtual ~background_handler() = default;
  [[nodiscard]] virtual bool is_active() = 0;
  [[nodiscard]] virtual bool set_wallpaper(std::string_view wallpaper,
                                           int position) = 0;
  [[nodiscard]] virtual bool set_background_color(unsigned long color) = 0;
  [[nodiscard]] std::string get_error() const;

  [[nodiscard]] static std::unique_ptr<background_handler> create();

protected:
  std::string m_error{};
};
} // namespace wallchanger
