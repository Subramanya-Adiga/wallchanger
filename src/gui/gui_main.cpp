#include "../platform/windows_helper.h"
#include "wall_app_gui.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  ENABLE_CRT_NEW
  wallchanger::gui::application app(argc, argv);
  return app.run();
}
