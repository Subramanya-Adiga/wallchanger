#pragma once
#include "change_server.h"
#include "log.h"
#include "platform/win32_service.h"

namespace wallchanger {

class change_service : public platform::win32::service_base {
  static constexpr std::string_view logger_name = "service_logger";

public:
  change_service() : service_base("My Change"), m_server(60000) {
    LOGGER_CREATE(logger_name);
    auto path = std::filesystem::path(log_directory()) /
                std::filesystem::path(logger_name);
    LOGGER_SET_FILE(logger_name, path.string() + ".txt");
  }

  ~change_service() override {
    CloseHandle(thread_signal);
    LOG_INFO(logger_name, "service closed");
  }

  change_service(const change_service &) = delete;
  change_service &operator=(const change_service &) = delete;
  change_service(change_service &&) = delete;
  change_service &operator=(change_service &&) = delete;

  void start([[maybe_unused]] DWORD control) override {
    LOG_INFO(logger_name, "service started");
    if (m_server.start()) {
      thread_signal = CreateEvent(nullptr, TRUE, FALSE, nullptr);

      bool quit = false;
      while (!quit) {
        m_server.update(-1, true);
        if (WaitForSingleObject(thread_signal, INFINITE) == WAIT_OBJECT_0) {
          quit = true;
        }
      }
    }
  }

  void stop([[maybe_unused]] DWORD control) override {
    SetEvent(thread_signal);
    m_server.stop();
    LOG_INFO(logger_name, "service stopped");
  }

private:
  HANDLE thread_signal = nullptr;
  change_server m_server;
};

} // namespace wallchanger