#ifdef _WINDOWS
#ifndef _DEBUG
#include "change_service.h"
#endif

#ifdef _DEBUG
#include "change_server.h"
namespace {
bool stop_server = false;
}

bool WINAPI close_handle([[maybe_unused]] DWORD sig) {
  stop_server = true;
  return true;
}

#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
#ifndef _DEBUG
  wallchanger::change_service service;
  wallchanger::platform::win32::service_base::run(service);
#else
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)close_handle, 1);
  wallchanger::change_server server(wallchanger::helper::s_port_number);
  bool running = server.start();
  if (running) {
    while (!stop_server) {
      server.update(-1, false);
    }
    server.store_state();
  }
#endif
  return 0;
}

#endif

#ifdef __linux__
#include "change_server.h"
namespace {
bool stop_server = false;
}

void app_exit([[maybe_unused]] int sig) { stop_server = true; }

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  wallchanger::change_server server(wallchanger::helper::s_port_number);

  if (!std::filesystem::exists(data_directory())) {
    std::filesystem::create_directories(data_directory());
  }

  if (!std::filesystem::exists(log_directory())) {
    std::filesystem::create_directories(log_directory());
  }

  bool running = server.start();
  (void)signal(SIGINT, app_exit);
  if (running) {
    while (!stop_server) {
      server.update(-1, false);
    }
    server.store_state();
  }

  return 0;
}
#endif