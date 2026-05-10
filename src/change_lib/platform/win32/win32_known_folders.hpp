#pragma once
#include "common.hpp"
#include <expected>
#include <filesystem>

//Forward Declaration Of Type Defined In known-folders.hpp
enum class Folders : u8;

std::expected<std::filesystem::path, std::error_code>
get_win32_path(Folders fld);
