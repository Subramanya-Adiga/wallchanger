#include "../platform/windows_helper.h"
#include "wall_app_gui.h"
#include "wall_app_qml.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  ENABLE_CRT_NEW
  wallchanger::gui::application app(argc, argv);
  // wallchanger::gui::application_qml app(argc, argv);
  return app.run();
}
