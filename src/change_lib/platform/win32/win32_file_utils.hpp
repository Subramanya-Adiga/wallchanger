#pragma once
#include "common.hpp"
#include <expected>
#include <string_view>

std::expected<HANDLE, std::error_code>
win32_open_file(std::string_view filename);
