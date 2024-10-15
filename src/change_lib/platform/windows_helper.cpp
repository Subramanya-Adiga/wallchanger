#include "windows_helper.hpp"
#include <bit>

std::wstring win32::to_utf16(const std::string &data) {
  if (data.empty()) {
    return {};
  }
  int size_needed = MultiByteToWideChar(
      CP_UTF8, 0, &data[0], static_cast<int>(data.size()), nullptr, 0);
  std::wstring to_str(static_cast<std::size_t>(size_needed), 0);
  if (MultiByteToWideChar(CP_UTF8, 0, &data[0], static_cast<int>(data.size()),
                          &to_str[0], size_needed) == 0) {
    win32::error_handler_win32::message_display("MultiByteToWideChar");
  }
  return to_str;
}

std::string win32::to_utf8(const std::wstring &data) {
  if (data.empty()) {
    return {};
  }
  int size_needed =
      WideCharToMultiByte(CP_UTF8, 0, &data[0], static_cast<int>(data.size()),
                          nullptr, 0, nullptr, nullptr);
  std::string to_str(static_cast<std::size_t>(size_needed), 0);
  if (WideCharToMultiByte(CP_UTF8, 0, &data[0], static_cast<int>(data.size()),
                          &to_str[0], size_needed, nullptr, nullptr) == 0) {
    win32::error_handler_win32::message_display("WideCharToMultiByte");
  }
  return to_str;
}

std::string win32::error_handler_win32::fmt_msg(std::string_view func_name,
                                                HRESULT status) {
  LPWSTR *m_msg_buf = nullptr;

  unsigned long id;
  if (status != S_OK) {
    id = status;
  } else {
    id = GetLastError();
  }

  if (m_msg_buf == nullptr) {
    m_msg_buf = static_cast<LPWSTR *>(LocalAlloc(LHND, ONE_KIB * KIB_64));
  }
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 nullptr, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 *m_msg_buf, ONE_KIB * KIB_64, nullptr);
  auto ret = fmt::format("{} failed with error {}: {}", func_name, id,
                         to_utf8(*m_msg_buf));
  LocalFree(m_msg_buf);
  return ret;
}

void win32::error_handler_win32::message_display(std::string_view func_name) {
  MessageBoxW(0, to_utf16(error_handler_win32::fmt_msg(func_name)).c_str(),
              L"Error", MB_ICONERROR | MB_OK);
}

#ifdef WIN32_SYSTEM_INFO

win32::wmi2::wmi2() {
  HRESULT res = 0;
  res = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(res)) {
  }

  res = CoInitializeSecurity(
      nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
  if (FAILED(res)) {
    CoUninitialize();
  }
  res = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IWbemLocator, std::bit_cast<LPVOID *>(&m_locator));

  if (FAILED(res)) {
    CoUninitialize();
  }

  res = m_locator->ConnectServer((wchar_t *)"ROOT\\CIMV2", nullptr, nullptr,
                                 nullptr, 0, nullptr, nullptr, &m_service);

  if (FAILED(res)) {
    m_locator->Release();
    CoUninitialize();
  }

  res = CoSetProxyBlanket(m_service, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                          nullptr, RPC_C_AUTHN_LEVEL_CALL,
                          RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

  if (FAILED(res)) {
    m_service->Release();
    m_locator->Release();
    CoUninitialize();
  }
}

win32::wmi2::~wmi2() {
  m_service->Release();
  m_locator->Release();
  CoUninitialize();
}

std::vector<win32::query_data>
win32::wmi2::query(const std::string &query_string,
                   const std::vector<std::string> &lists) {
  HRESULT res = 0;
  IEnumWbemClassObject *enumerator = nullptr;
  res = m_service->ExecQuery(
      (wchar_t *)"WQL", std::bit_cast<wchar_t *>(query_string.c_str()),
      WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
      &enumerator);
  if (FAILED(res)) {
    return {};
  }
  IWbemClassObject *class_obj = nullptr;
  ULONG returned = 0;
  std::vector<win32::query_data> out;
  out.reserve(1);
  while (enumerator != nullptr) {
    res = enumerator->Next(WBEM_INFINITE, 1, &class_obj, &returned);
    if (returned == 0) {
      break;
    }
    VARIANT var_data;
    CIMTYPE var_type = 0;
    for (auto &&filter : lists) {
      res = class_obj->Get(win32::to_utf16(filter).c_str(), 0, &var_data,
                           &var_type, nullptr);
      if (FAILED(res)) {
        continue;
      }
      if (var_data.vt == VT_NULL) {
        continue;
      }
      if ((var_type == CIM_STRING || var_type == CIM_REFERENCE) &&
          (var_type != CIM_EMPTY && var_type != CIM_ILLEGAL)) {
        out.emplace_back(win32::to_utf8(var_data.bstrVal));
      }
      if ((var_type == CIM_UINT32 || var_type == CIM_UINT64 ||
           var_type == CIM_REFERENCE) &&
          (var_type != CIM_EMPTY && var_type != CIM_ILLEGAL)) {
        out.emplace_back(var_data.uintVal);
      }
      VariantClear(&var_data);
    }
  }
  class_obj->Release();
  enumerator->Release();
  return out;
}
#endif

#ifdef WIN32_UUID_GEN

std::string win32::get_uuid() {
  UUID id;
  if (auto status = UuidCreate(&id); status == RPC_S_OK) {
    const char *str = nullptr;
    if (auto status_str = UuidToString(&id, std::bit_cast<RPC_CSTR *>(&str));
        status_str == RPC_S_OK) {
      std::string ret(str);
      RpcStringFree(std::bit_cast<RPC_CSTR *>(&str));
      return ret;
    }
    return {};
  }
  return {};
}

#endif

#ifdef WIN32_RANDOM_GENERATOR

std::vector<uint8_t> win32::get_random_data(int counter) {
  if (!IsWindows7OrGreater()) {
    return p_get_random_data_old(counter);
  }
  return p_get_random_data_new(counter);
}

std::vector<uint8_t> win32::p_get_random_data_old(int counter = 1) {
  HCRYPTPROV cprovider = 0;
  std::vector<uint8_t> ret(KIB_256 * counter);
  if (CryptAcquireContext(&cprovider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT)) {
    for (int i = 0; i < counter; i++) {
      if (!CryptGenRandom(cprovider, KIB_256 * counter, &ret[0])) {
        error_handler_win32().message_display("CryptGenRandom");
        ret.clear();

        if (!CryptReleaseContext(cprovider, 0)) {
          error_handler_win32().message_display("CryptReleaseContext");
          ret.clear();
          return ret;
        }
        return ret;
      }
    }
    if (!CryptReleaseContext(cprovider, 0))
      error_handler_win32().message_display("CryptReleaseContext");
    return ret;
  } else {
    error_handler_win32().message_display("CryptAquireContextA");
    return ret;
  }
}

std::vector<uint8_t> win32::p_get_random_data_new(int counter = 1) {
  std::vector<uint8_t> ret(KIB_256 * counter);
  BCRYPT_ALG_HANDLE *ahandle = nullptr;
  if (BCryptOpenAlgorithmProvider(ahandle, BCRYPT_RNG_ALGORITHM, nullptr, 0) ==
          0 ||
      STATUS_INVALID_PARAMETER) {
    for (int i = 0; i < counter; i++) {
      BCryptGenRandom(ahandle, ret.data(), KIB_256 * counter,
                      BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    }
    BCryptCloseAlgorithmProvider(ahandle, 0);
    return ret;
  } else {
    error_handler_win32().message_display("BCryptOpenAlgorithmProvider");
    return {};
  }
}

#endif

#ifdef WIN32_SYSTEM_INFO

void win32::system_info::get_gpuinfo(gpu_info_struct &info) {
  IDXGIFactory *factory = nullptr;
  IDXGIAdapter *adapter = nullptr;
  DXGI_ADAPTER_DESC desc;

  if (HRESULT hres = CreateDXGIFactory(__uuidof(IDXGIFactory),
                                       std::bit_cast<void **>(&factory));
      hres < 0) {
    // error
  }
  if (HRESULT hres = factory->EnumAdapters(0, &adapter); hres < 0) {
    // error
  }
  if (HRESULT hres = adapter->GetDesc(&desc); hres < 0) {
    // error
  }

  std::vector<std::string> list = {"Name", "CurrentHorizontalResolution",
                                   "CurrentVerticalResolution",
                                   "MaxRefreshRate", "CurrentBitsPerPixel"};
  auto ret = m_wmi.query("SELECT * FROM Win32_VideoController", list);
  int counter = 0;
  for (auto &&data : ret) {
    std::visit(overloaded{
                   [&](unsigned int arg) {
                     switch (counter) {
                     case 1:
                       info.resolution_horizontal = arg;
                       break;
                     case 2:
                       info.resolution_vertical = arg;
                       break;
                     case 3:
                       info.refresh_rate = arg;
                       break;
                     case 4:
                       info.bits_pre_pixal = arg;
                       break;
                     default:
                       break;
                     }
                   },
                   [&](const std::string &arg) { info.name = arg; },
               },
               data);
    counter += 1;
    info.memory = desc.DedicatedVideoMemory;
  }
}

void win32::system_info::get_processor_info(win32::cpu_info_struct &info) {

  std::vector<std::string> list = {
      "Name",        "SocketDesignation", "Manufacturer", "NumberOfCores",
      "ThreadCount", "CurrentClockSpeed", "AddressWidth", "Architecture"};
  auto ret = m_wmi.query("SELECT * FROM Win32_Processor", list);
  int counter = 0;
  for (auto &&data : ret) {
    std::visit(overloaded{
                   [&](unsigned int arg) {
                     switch (counter) {
                     case 3:
                       info.core_count = arg;
                       break;
                     case 4:
                       info.thread_count = arg;
                       break;
                     case 5:
                       info.clock_speed = arg;
                       break;
                     case 6:
                       info.addresswidth = static_cast<uint16_t>(arg);
                       break;
                     case 7:
                       info.architecture = static_cast<uint16_t>(arg);
                       break;
                     default:
                       break;
                     }
                   },
                   [&](const std::string &arg) {
                     switch (counter) {
                     case 0:
                       info.name = arg;
                       break;
                     case 1:
                       info.socket = arg;
                       break;
                     case 2:
                       info.manufacturer = arg;
                       break;
                     default:
                       break;
                     }
                   },
               },
               data);
    counter += 1;
  }
}

void win32::system_info::getcomputer_info(win32::computer_info &info) {

  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (!GlobalMemoryStatusEx(&statex)) {
    error_handler_win32().message_display("GlobalMemoryStatusEx");
  }

  auto board_ret = m_wmi.query("SELECT * FROM Win32_BaseBoard", {"Product"});
  info.mother_board = std::get<std::string>(board_ret[0]);

  auto computer_system_ret = m_wmi.query("SELECT * FROM Win32_ComputerSystem",
                                         {"TotalPhysicalMemory"});
  info.total_physical_memory =
      std::get<unsigned int>(computer_system_ret[0]) / ONE_KIB;
  info.total_virtual_memory = statex.ullTotalVirtual / ONE_KIB;

  auto physical_mem_ret =
      m_wmi.query("SELECT * FROM Win32_PhysicalMemory", {"Speed"});
  info.memory_speed = std::get<unsigned int>(physical_mem_ret[0]);

  auto operating_system_ret = m_wmi.query(
      "SELECT * FROM Win32_OperatingSystem",
      {"Name", "BuildNumber", "CodeSet", "Version", "OSArchitecture",
       "ServicePackMinorVersion", "ServicePackMajorVersion"});
  int counter = 0;
  for (auto &&data : operating_system_ret) {
    std::visit(overloaded{[&](unsigned int arg) {
                            switch (counter) {
                            case 5:
                              info.service_pack_minor =
                                  static_cast<uint16_t>(arg);
                              break;
                            case 6:
                              info.service_pack_major =
                                  static_cast<uint16_t>(arg);
                              break;
                            default:
                              break;
                            }
                          },
                          [&](const std::string &arg) {
                            switch (counter) {
                            case 0:
                              info.name = arg;
                              break;
                            case 1:
                              info.build_number = arg;
                              break;
                            case 2:
                              info.codepage = arg;
                              break;
                            case 3:
                              info.version = arg;
                              break;
                            case 4:
                              info.os_arch = arg;
                              break;
                            default:
                              break;
                            }
                          }},
               data);
    counter += 1;
  }
}

#endif
