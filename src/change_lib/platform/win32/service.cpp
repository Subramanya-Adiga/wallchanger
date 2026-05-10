#include "win32_service.hpp"

#include "windows_helper.hpp"
#include <cstddef>

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

std::string_view
wallchanger::platform::win32::service_base::get_service_name() const {
  return service_name;
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

wallchanger::platform::win32::service_helper::service_helper(
    const std::string &name)
    : m_name(name) {
  if (m_sc_manager_handle =
          OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
      m_sc_manager_handle == nullptr) {
    fmt::print("{}\n", ::win32::error_handler_win32::fmt_msg("OpenSCManager"));
  }
}

wallchanger::platform::win32::service_helper::~service_helper() {
  if (m_service_handle != nullptr) {
    CloseHandle(m_service_handle);
  }
  if (m_sc_manager_handle != nullptr) {
    CloseHandle(m_sc_manager_handle);
  }
}

void wallchanger::platform::win32::service_helper::install_service(
    const std::string &description) {
  std::array<TCHAR, MAX_PATH> szUnquotedPath{};

  if (!GetModuleFileName(nullptr, szUnquotedPath.data(),
                         static_cast<DWORD>(szUnquotedPath.size()))) {
    fmt::print("{}\n",
               ::win32::error_handler_win32::fmt_msg("GetModuleFileName"));
    return;
  }

  auto fmt =
      fmt::format("\"{}\"", ::win32::to_utf8(szUnquotedPath.data()).c_str());
  if (m_service_handle = CreateService(
          m_sc_manager_handle, ::win32::to_utf16(m_name).c_str(),
          ::win32::to_utf16(description).c_str(), SERVICE_ALL_ACCESS,
          SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
          ::win32::to_utf16(fmt).c_str(), nullptr, nullptr, nullptr, nullptr,
          nullptr);
      m_service_handle == nullptr) {
    fmt::print("{}\n", ::win32::error_handler_win32::fmt_msg("CreateService"));
    return;
  } else {
    fmt::print("Service installed successfully\n");
  }
}

void wallchanger::platform::win32::service_helper::query_service_info() {
  LPQUERY_SERVICE_CONFIG lpsc{};
  LPSERVICE_DESCRIPTION lpsd{};
  DWORD dwBytesNeeded{};
  DWORD cbBufSize{};

  m_service_handle = m_open_service(SERVICE_QUERY_CONFIG);

  if (!QueryServiceConfig(m_service_handle, nullptr, 0, &dwBytesNeeded)) {
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      cbBufSize = dwBytesNeeded;
      lpsc = std::bit_cast<LPQUERY_SERVICE_CONFIG>(
          LocalAlloc(LMEM_FIXED, cbBufSize));
    } else {
      fmt::print("{}\n",
                 ::win32::error_handler_win32::fmt_msg("QueryServiceConfig"));
      return;
    }
  }

  if (!QueryServiceConfig(m_service_handle, lpsc, cbBufSize, &dwBytesNeeded)) {
    fmt::print("{}\n",
               ::win32::error_handler_win32::fmt_msg("QueryServiceConfig"));
    return;
  }

  if (!QueryServiceConfig2(m_service_handle, SERVICE_CONFIG_DESCRIPTION,
                           nullptr, 0, &dwBytesNeeded)) {
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      cbBufSize = dwBytesNeeded;
      lpsd = std::bit_cast<LPSERVICE_DESCRIPTION>(
          LocalAlloc(LMEM_FIXED, cbBufSize));
    } else {
      fmt::print("{}\n",
                 ::win32::error_handler_win32::fmt_msg("QueryServiceConfig2"));
      return;
    }
  }

  if (!QueryServiceConfig2(m_service_handle, SERVICE_CONFIG_DESCRIPTION,
                           std::bit_cast<LPBYTE>(lpsd), cbBufSize,
                           &dwBytesNeeded)) {
    fmt::print("{}\n",
               ::win32::error_handler_win32::fmt_msg("QueryServiceConfig2"));
    return;
  }

  fmt::print("{} configuration:\n Type:{:X}\n Start Type:{:X}\n Error "
             "Control:{:X}\n BinaryPath:{}\n Account:{}\n ",
             m_name, lpsc->dwServiceType, lpsc->dwStartType,
             lpsc->dwErrorControl, ::win32::to_utf8(lpsc->lpBinaryPathName),
             ::win32::to_utf8(lpsc->lpServiceStartName));

  if (lpsd->lpDescription != nullptr) {
    fmt::print("Description:{}\n ", ::win32::to_utf8(lpsd->lpDescription));
  }

  if (lpsc->lpLoadOrderGroup != nullptr) {
    fmt::print("Load Order Group:{}\n",
               ::win32::to_utf8(lpsc->lpLoadOrderGroup));
  }

  if (lpsc->dwTagId != 0) {
    fmt::print("Tag ID:{}\n ", lpsc->dwTagId);
  }

  if (lpsc->lpDependencies != nullptr) {
    fmt::print(" Dependencies:{}\n", ::win32::to_utf8(lpsc->lpDependencies));
  }

  LocalFree(lpsc);
  LocalFree(lpsd);
}

void wallchanger::platform::win32::service_helper::delete_service() {
  m_service_handle = m_open_service(DELETE);

  if (DeleteService(m_service_handle) == 0) {
    fmt::print("{}\n", ::win32::error_handler_win32::fmt_msg("DeleteService"));
  } else {
    fmt::print("{} service deleted succesfully", m_name);
  }
}

wallchanger::platform::win32::service_helper::service_state
wallchanger::platform::win32::service_helper::query_service_state() {
  LPSERVICE_STATUS status_info{};
  m_service_handle = m_open_service(SERVICE_QUERY_STATUS);
  status_info = std::bit_cast<LPSERVICE_STATUS>(
      LocalAlloc(LMEM_FIXED, sizeof(LPSERVICE_STATUS)));
  if (QueryServiceStatus(m_service_handle, status_info) == 0) {
    fmt::print("{}\n",
               ::win32::error_handler_win32::fmt_msg("QueryServiceStatus"));
    LocalFree(status_info);
    return {};
  } else {
    LocalFree(status_info);
    return static_cast<service_helper::service_state>(
        status_info->dwCurrentState);
  }
}

bool wallchanger::platform::win32::service_helper::is_installed() {
  DWORD buffer_size{};
  DWORD bytes_required{};
  DWORD total_services{};
  DWORD resume_handle = 0;
  LPENUM_SERVICE_STATUS service_status{};
  std::vector<std::string> vec{};

  bool found = false;

  if (EnumServicesStatus(m_sc_manager_handle, SERVICE_WIN32_OWN_PROCESS,
                         SERVICE_STATE_ALL, nullptr, 0, &bytes_required,
                         &total_services, nullptr) == 0) {
    buffer_size = bytes_required;
  } else {
    fmt::print("{}\n",
               ::win32::error_handler_win32::fmt_msg("EnumServicesStatus"));
  }

  service_status =
      std::bit_cast<LPENUM_SERVICE_STATUS>(LocalAlloc(LMEM_FIXED, buffer_size));

  auto enum_services = [&]() -> int {
    bool more = false;
    if (auto ret = EnumServicesStatus(m_sc_manager_handle, SERVICE_WIN32,
                                      SERVICE_STATE_ALL, service_status,
                                      buffer_size, &bytes_required,
                                      &total_services, &resume_handle);
        ret == 0) {
      if (GetLastError() == ERROR_MORE_DATA) {
        more = true;
      }
    } else {
      fmt::print("{}\n",
                 ::win32::error_handler_win32::fmt_msg("EnumServicesStatus"));
      return 0;
    }
    for (DWORD i = 0; i < total_services; i++) {
      vec.push_back(::win32::to_utf8(service_status[i].lpServiceName));
    }
    return more ? ERROR_MORE_DATA : 0;
  };

  bool quit = false;

  do {
    if (enum_services() == 0 || ERROR_ACCESS_DENIED) {
      quit = true;
    }
  } while (!quit);

  auto rng_it = std::ranges::find(vec, m_name);
  if (rng_it != std::ranges::end(vec)) {
    found = true;
  }

  LocalFree(service_status);
  return found;
}

SC_HANDLE
wallchanger::platform::win32::service_helper::m_open_service(DWORD Access) {
  SC_HANDLE handle{};
  if (handle = OpenService(m_sc_manager_handle,
                           ::win32::to_utf16(m_name).c_str(), Access);
      handle == nullptr) {
    fmt::print("{}\n", ::win32::error_handler_win32::fmt_msg("OpenService"));
    return {};
  }
  return handle;
}