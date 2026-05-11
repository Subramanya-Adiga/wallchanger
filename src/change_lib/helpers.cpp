#include "helpers.hpp"

#include "known-folders.hpp"

std::string data_directory() {
#ifdef OS_WINDOWS
  auto ret = get_path(Folders::local_configuration)->string();
#endif
#ifdef OS_LINUX
  auto ret = get_path(Folders::data)->string();
#endif
  return ret + "/wallchanger";
}

std::string config_directory() {
  auto ret = get_path(Folders::local_configuration)->string();
  return ret + "/wallchanger";
}
