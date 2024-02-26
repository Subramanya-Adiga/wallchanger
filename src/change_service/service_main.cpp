#ifdef _WINDOWS
#include "change_service.h"

#ifdef _DEBUG
wallchanger::change_service service_debug;
bool WINAPI close_handle(DWORD /*sig*/) {
  service_debug.stop(0);
  return true;
}
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
#ifndef _DEBUG
  wallchanger::change_service service;
  wallchanger::platform::win32::service_base::run(service);
#else
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)close_handle, 1);
  service_debug.start(0);
#endif
  return 0;
}
#endif

#ifdef __linux__
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) { return 0; }
#endif