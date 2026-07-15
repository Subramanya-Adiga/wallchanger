#pragma once
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DBG_NEW new
#endif
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#ifdef _DEBUG
#define ENABLE_CRT_NEW                                                         \
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
#define ENABLE_CRT_NEW
#endif

#ifdef NOMINMAX
#include <windows.h>
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#ifdef WIN32_SYSTEM_INFO

#include <comdef.h>
#include <cstdlib>
#include <d3d11.h>
#include <dxgi.h>
// #include <ntstatus.h>
#include <versionhelpers.h>
#include <wbemcli.h>

#endif

namespace win32 {
#ifdef WIN32_SYSTEM_INFO

using query_obj = std::unordered_map<std::string, std::string>;
// using query_obj = std::variant<std::string, unsigned int>;
using query_data = std::variant<std::string, unsigned int>;

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif

constexpr static int ONE_KIB = 1024;
constexpr static int KIB_64 = 64;
constexpr static int KIB_256 = 256;

struct error_handler_win32 {
  [[nodiscard]] static std::string fmt_msg(std::string_view func_name,
                                           HRESULT status = S_OK);

  static void message_display(std::string_view func_name);
};

[[nodiscard]] std::string to_utf8(const std::wstring &data);
[[nodiscard]] std::wstring to_utf16(const std::string &data);

#ifdef WIN32_SYSTEM_INFO

class wmi2 {
  IWbemLocator *m_locator = nullptr;
  IWbemServices *m_service = nullptr;

public:
  wmi2();
  ~wmi2();

  wmi2(wmi2 &&) = delete;
  wmi2(const wmi2 &) = delete;
  wmi2 &operator=(wmi2 &&) = delete;
  wmi2 &operator=(const wmi2 &) = delete;

  [[nodiscard]] std::vector<query_data>
  query(const std::string &query_string, const std::vector<std::string> &lists);
};

struct gpu_info_struct {
  std::string name;
  size_t memory;
  uint32_t resolution_horizontal;
  uint32_t resolution_vertical;
  uint32_t refresh_rate;
  uint32_t bits_pre_pixal;
};

struct cpu_info_struct {
  std::string name;
  std::string manufacturer;
  std::string socket;
  uint32_t core_count;
  uint32_t thread_count;
  uint16_t architecture;
  uint16_t addresswidth;
  uint32_t clock_speed;
};

struct computer_info {
  std::string mother_board;
  uint64_t total_physical_memory;
  uint64_t total_virtual_memory;
  uint32_t memory_speed;
  std::string name;
  std::string build_number;
  std::string codepage;
  std::string version;
  std::string os_arch;
  uint16_t service_pack_minor;
  uint16_t service_pack_major;
};

struct system_info {
  void get_gpuinfo(gpu_info_struct &info);
  void get_processor_info(cpu_info_struct &info);
  void getcomputer_info(computer_info &info);

private:
  wmi2 m_wmi;
};

#endif

#ifdef WIN32_UUID_GEN

[[nodiscard]] std::string get_uuid();

#endif

#ifdef WIN32_RANDOM_GENERATOR

[[nodiscard]] std::vector<uint8_t> get_random_data(int counter);
[[nodiscard]] std::vector<uint8_t> p_get_random_data_old(int counter);
[[nodiscard]] std::vector<uint8_t> p_get_random_data_new(int counter);

#endif
} // namespace win32
