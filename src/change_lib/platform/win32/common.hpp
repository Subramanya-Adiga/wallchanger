#pragma once
#include "../../defines.hpp"
#include <system_error>

struct win32_error_code {
  explicit win32_error_code(u32 e) noexcept : error(e) {}
  u32 error;
};

namespace std {
template <> struct is_error_code_enum<win32_error_code> : std::true_type {};
} // namespace std

std::error_code make_error_code(const win32_error_code &e);

std::error_code make_win32_error_code();

std::error_code make_HRESULT_error_code(HRESULT e);

std::string to_utf8(LPWSTR msg, u32 len);

std::wstring to_wtf8(LPCSTR msg, u32 len);
