#include "win32_service.h"
#include "windows_helper.h"

wallchanger::platform::win32::service_base
    *wallchanger::platform::win32::service_base::service = nullptr;

wallchanger::platform::win32::service_base::service_base(std::string name)
    : service_name(std::move(name)) {}

void WINAPI
wallchanger::platform::win32::service_base::run(service_base &runnable) {
  service = &runnable;

  std::vector<SERVICE_TABLE_ENTRY> DispatchTable = {
      {std::bit_cast<LPWSTR>(service->service_name.c_str()),
       (LPSERVICE_MAIN_FUNCTION)service_main},
      {nullptr, nullptr}};

  if (!StartServiceCtrlDispatcher(DispatchTable.data())) {
    service->report_event("StartServiceCtrlDispatcher");
  }
}

void wallchanger::platform::win32::service_base::init_service(
    [[maybe_unused]] DWORD dwArgc, [[maybe_unused]] LPTSTR *lpszArgv) {
  SvcStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

  if (SvcStopEvent == nullptr) {
    report_svc_status(SERVICE_STOPPED, GetLastError(), 0);
    return;
  }

  // Report running status when initialization is complete.

  report_svc_status(SERVICE_RUNNING, NO_ERROR, 0);

  // TO_DO: Perform work until service stops.

  service->start(0);

  // while (true) {
  //  Check whether to stop the service.

  WaitForSingleObject(SvcStopEvent, INFINITE);

  report_svc_status(SERVICE_STOPPED, NO_ERROR, 0);
  //  }
}

void WINAPI wallchanger::platform::win32::service_base::service_main(
    DWORD dwArgc, LPWSTR *lpszArgv) {
  // Register the handler function for the service

  service->SvcStatusHandle = RegisterServiceCtrlHandler(
      ::win32::to_utf16(service->service_name).c_str(),
      service_control_handler);

  if (service->SvcStatusHandle == nullptr) {
    service->report_event("RegisterServiceCtrlHandler");
    return;
  }

  // These SERVICE_STATUS members remain as set here

  service->SvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service->SvcStatus.dwServiceSpecificExitCode = 0;
  service->SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE |
                                          SERVICE_ACCEPT_STOP |
                                          SERVICE_ACCEPT_SHUTDOWN;

  // Report initial status to the SCM

  service->report_svc_status(SERVICE_START_PENDING, NO_ERROR, 3000);

  // Perform service-specific initialization and work.

  service->init_service(dwArgc, lpszArgv);
}

void WINAPI wallchanger::platform::win32::service_base::service_control_handler(
    DWORD control) {
  // Handle the requested control code.

  switch (control) {
  case SERVICE_CONTROL_SHUTDOWN:
  case SERVICE_CONTROL_STOP: {
    service->report_svc_status(SERVICE_STOP_PENDING, NO_ERROR, 0);
    // Signal the service to stop.
    service->stop(control);
    SetEvent(service->SvcStopEvent);
    service->report_svc_status(service->SvcStatus.dwCurrentState, NO_ERROR, 0);
    break;
  }

  case SERVICE_CONTROL_CONTINUE: {
    service->report_svc_status(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);
    service->start(control);
    service->report_svc_status(SERVICE_RUNNING, NO_ERROR, 0);
    break;
  }

  case SERVICE_CONTROL_PAUSE: {
    service->report_svc_status(SERVICE_PAUSE_PENDING, NO_ERROR, 0);
    service->stop(control);
    service->report_svc_status(SERVICE_PAUSED, NO_ERROR, 0);
    break;
  }

  default:
    break;
  }
}

void wallchanger::platform::win32::service_base::report_svc_status(
    DWORD CurrentState, DWORD Win32ExitCode, DWORD WaitHint) {
  static DWORD dwCheckPoint = 1;

  // Fill in the SERVICE_STATUS structure.

  SvcStatus.dwCurrentState = CurrentState;
  SvcStatus.dwWin32ExitCode = Win32ExitCode;
  SvcStatus.dwWaitHint = WaitHint;

  if (CurrentState == SERVICE_START_PENDING) {
    SvcStatus.dwControlsAccepted = 0;
  } else {
    SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  }

  if ((CurrentState == SERVICE_RUNNING) || (CurrentState == SERVICE_STOPPED)) {
    SvcStatus.dwCheckPoint = 0;
  } else {
    SvcStatus.dwCheckPoint = dwCheckPoint++;
  }

  // Report the status of the service to the SCM.
  SetServiceStatus(SvcStatusHandle, &SvcStatus);
}

void wallchanger::platform::win32::service_base::report_event(
    std::string_view func_name) const {
  HANDLE event_handle = nullptr;
  std::array<LPCWSTR, 2> strings{};
  event_handle =
      RegisterEventSource(nullptr, ::win32::to_utf16(service_name).c_str());
  if (event_handle != nullptr) {
    strings[0] = ::win32::to_utf16(service_name).c_str();
    strings[1] =
        ::win32::to_utf16(::win32::error_handler_win32::fmt_msg(func_name))
            .c_str();
    ReportEvent(event_handle, EVENTLOG_ERROR_TYPE, 0, SVC_ERROR, nullptr, 2, 0,
                strings.data(), nullptr);
    DeregisterEventSource(event_handle);
  }
}

void wallchanger::platform::win32::service_base::install_service() const {
  SC_HANDLE schSCManager = nullptr;
  SC_HANDLE schService = nullptr;
  std::array<TCHAR, MAX_PATH> szUnquotedPath{};

  if (!GetModuleFileName(nullptr, szUnquotedPath.data(),
                         static_cast<DWORD>(szUnquotedPath.size()))) {
    fmt::print("Cannot install service {}\n", GetLastError());
    return;
  }

  auto fmt =
      fmt::format("\"{}\"", ::win32::to_utf8(szUnquotedPath.data()).c_str());
  schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

  if (nullptr == schSCManager) {
    fmt::print("OpenSCManager failed {}\n", GetLastError());
    return;
  }

  schService =
      CreateService(schSCManager, ::win32::to_utf16(service_name).c_str(),
                    ::win32::to_utf16(service_name).c_str(), SERVICE_ALL_ACCESS,
                    SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
                    SERVICE_ERROR_NORMAL, ::win32::to_utf16(fmt).c_str(),
                    nullptr, nullptr, nullptr, nullptr, nullptr);

  if (schService == nullptr) {
    fmt::print("CreateService failed {}\n", GetLastError());
    CloseServiceHandle(schSCManager);
    return;
  } else {
    fmt::print("Service installed successfully\n");
  }

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);
}