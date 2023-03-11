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

  void start([[maybe_unused]] DWORD control) override {
    LOG_INFO(logger_name, "service started");
    if (m_server.start()) {
      m_exec = std::thread([&]() {
        while (!m_stop) {
          m_server.update(-1, false);
        }
        m_server.stop();
      });
    }
  }

  void stop([[maybe_unused]] DWORD control) override {
    m_stop = true;
    if (m_exec.joinable()) {
      m_exec.join();
    }
    LOG_INFO(logger_name, "service stopped");
  }

private:
  change_server m_server;
  std::thread m_exec;
  std::atomic_bool m_stop = false;
};

} // namespace wallchanger