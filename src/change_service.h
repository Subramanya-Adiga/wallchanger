#pragma once
#include "log.h"
#include "platform/win32_service.h"

namespace wallchanger {

class change_service : public platform::win32::service_base {
public:
  change_service() : service_base("My Change") {
    LOGGER_CREATE("Service_Logger");
    LOGGER_SET_FILE("Service_Logger",
                    std::filesystem::path("D:/servicelogger.txt").string());
  }
  ~change_service() override {
    CloseHandle(thread_handle);
    LOG_INFO("Service_Logger", "thread closed");
    CloseHandle(thread_signal);
    LOG_INFO("Service_Logger", "service closed");
  }
  change_service(const change_service &) = delete;
  change_service &operator=(const change_service &) = delete;
  change_service(change_service &&) = default;
  change_service &operator=(change_service &&) = default;

  void start(DWORD control) override {

    LOG_INFO("Service_Logger", "service started");

    thread_signal = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    thread_handle = CreateThread(nullptr, 0, &change_service::worker,
                                 thread_signal, 0, nullptr);

    WaitForSingleObject(thread_handle, INFINITE);
    WaitForSingleObject(thread_signal, INFINITE);
  }

  void stop(DWORD control) override {
    SetEvent(thread_signal);
    LOG_INFO("Service_Logger", "service stopped");
  }

private:
  HANDLE thread_handle = nullptr;
  HANDLE thread_signal = nullptr;

  static DWORD WINAPI worker(LPVOID lparam) {
    //  Periodically check if the service has been requested to stop
    LOG_INFO("Service_Logger", "thread_started");
    while (WaitForSingleObject(static_cast<HANDLE>(lparam), 0) !=
           WAIT_OBJECT_0) {
      LOG_INFO("Service_Logger", "thread_running");
      Sleep(3000);
    }
    LOG_INFO("Service_Logger", "thread_stopped");
    return ERROR_SUCCESS;
  }
};

} // namespace wallchanger