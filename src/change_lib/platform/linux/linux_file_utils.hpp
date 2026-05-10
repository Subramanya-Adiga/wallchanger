#pragma once
#include <expected>

class FileHandle;

std::expected<FileHandle,std::error_code> linux_open_file(const char* file_name);
