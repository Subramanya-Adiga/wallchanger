#include "common.hpp"

namespace {
class win32_error_cat : public std::error_category {
public:
  [[nodiscard]] const char *name() const noexcept override {
    return "Win32Error";
  }
  [[nodiscard]] std::string message(int c) const override {
    auto errorno = static_cast<u32>(c);
    auto alloc_size = static_cast<usize>(1024 * 64);
    auto *error = static_cast<LPWSTR>(
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, alloc_size));

    if (error == nullptr) {
      return "N/A";
    }

    auto len = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        GetModuleHandle(L"ntdll.dll"), errorno,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, alloc_size, nullptr);

    if (len == 0) {
      HeapFree(GetProcessHeap(), 0, error);
      return "N/A";
    }

    auto ret = to_utf8(error, len + 1);
    HeapFree(GetProcessHeap(), 0, error);
    return ret;
  };
};
const win32_error_cat error_cat{};
} // namespace

std::error_code make_error_code(const win32_error_code &e) {
  return {static_cast<int>(e.error), error_cat};
}

std::error_code make_win32_error_code() {
  return make_error_code(win32_error_code(GetLastError()));
}

std::error_code make_HRESULT_error_code(HRESULT e) {
  return make_error_code(win32_error_code(static_cast<u32>(e)));
}

std::string to_utf8(LPWSTR msg, u32 len) {
  if (msg == nullptr) {
    return "N/A";
  }

  auto required_len =
      WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, msg,
                          static_cast<int>(len), nullptr, 0, nullptr, nullptr);

  std::string to_str(static_cast<usize>(required_len), 0);
  (void)WideCharToMultiByte(CP_UTF8, 0, msg, static_cast<int>(len), &to_str[0],
                            required_len, nullptr, nullptr);
  return to_str;
}

std::wstring to_wtf8(LPCSTR msg, u32 len) {
  if (msg == nullptr) {
    return L"N/A";
  }

  auto required_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msg,
                                          static_cast<int>(len), nullptr, 0);

  std::wstring ret(static_cast<usize>(required_len), 0);

  (void)MultiByteToWideChar(CP_UTF8, 0, msg, static_cast<int>(len), &ret[0],
                            required_len);
  return ret;
}
