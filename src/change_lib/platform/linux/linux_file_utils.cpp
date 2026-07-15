#include "linux_file_utils.hpp"
#include "../../file_utils.hpp"
#include "common.hpp"
#include <fcntl.h>

std::expected<FileHandle, std::error_code>
linux_open_file(const char *file_name) {
  i8 file_desc = -1;
  if (file_desc = open(file_name, 0, 0)) {
  }
  return {FileHandle{static_cast<u64>(file_desc)}};
}
