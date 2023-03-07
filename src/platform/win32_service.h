#pragma once

namespace wallchanger::platform::win32 {
#define SVC_ERROR ((DWORD)0xC0020001L)

class service_base {

  std::string service_name;
  HANDLE SvcStopEvent = nullptr;
  SERVICE_STATUS SvcStatus{};
  SERVICE_STATUS_HANDLE SvcStatusHandle{};
  static service_base *service;

public:
  service_base(const service_base &) = delete;
  service_base &operator=(const service_base &) = delete;

  virtual ~service_base() = default;
  static void WINAPI run(service_base &runnable);

  virtual void start(DWORD) = 0;
  virtual void stop(DWORD) = 0;

  [[nodiscard]] std::string_view get_service_name() const;

protected:
  explicit service_base(std::string name);
  service_base(service_base &&) = default;
  service_base &operator=(service_base &&) = default;

  void init_service([[maybe_unused]] DWORD dwArgc,
                    [[maybe_unused]] LPTSTR *lpszArgv);

  static void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);

  static void WINAPI service_control_handler(DWORD control);

  void report_svc_status(DWORD CurrentState, DWORD Win32ExitCode,
                         DWORD WaitHint);

  void report_event(std::string_view func_name) const;
};

class service_helper {
public:
  enum class service_state : uint32_t {
    CONTINUE_PENDING,
    PAUSE_PENDING,
    PAUSED,
    RUNNING,
    START_PENDING,
    STOP_PENDING,
    STOPPED
  };

  explicit service_helper(const std::string &name);
  ~service_helper();

  void install_service(const std::string &description);
  void query_service_info();
  void delete_service();
  [[nodiscard]] service_state query_service_state();
  [[nodiscard]] bool is_installed();

private:
  SC_HANDLE m_sc_manager_handle{};
  SC_HANDLE m_service_handle{};
  std::string m_name;
  SC_HANDLE m_open_service(DWORD Access);
};
} // namespace wallchanger::platform::win32
