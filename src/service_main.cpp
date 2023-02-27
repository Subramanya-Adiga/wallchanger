#include "change_service.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  wallchanger::change_service service;
  wallchanger::platform::win32::service_base::run(service);
  return 0;
}
