#pragma once
#include "../../defines.hpp"
#include <expected>
#include <system_error>

enum class Folders : u8;

std::expected<std::filesystem::path, std::error_code>
linux_get_path(Folders fld);
