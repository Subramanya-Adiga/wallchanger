#include "linux_known_folders.hpp"

#include "../../known-folders.hpp"

#include <filesystem>
#include <fstream>
#include <linux/unistd.h>
#include <pwd.h>
#include <ranges>
#include <string>
#include <unistd.h>

namespace {
std::unordered_map<u8, std::string> lookup;

std::string get_home() {
  auto uid = getuid();
  auto *home = getenv("HOME");

  if (uid != 0 && (home != nullptr)) {
    return home;
  }

  passwd *pw = nullptr;
  passwd pwd = {};

  auto bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize < 1) {
    bufsize = 16384;
  }

  auto *buffer = (char *)malloc(bufsize);

  auto err = getpwuid_r(uid, &pwd, buffer, bufsize, &pw);
  if (err == ERANGE) {
    bufsize *= 2;
    buffer = nullptr;
    buffer = (char *)malloc(bufsize);
    err = getpwuid_r(uid, &pwd, buffer, bufsize, &pw);
  }
  std::string res_dir = pw->pw_dir;

  free(buffer);

  return res_dir;
}

void init() {
  lookup.emplace(static_cast<u8>(Folders::home), get_home());
  lookup.emplace(static_cast<u8>(Folders::cache), get_home() + "/.cache");
  lookup.emplace(static_cast<u8>(Folders::local_configuration),
                 get_home() + "/.config");
  lookup.emplace(static_cast<u8>(Folders::roaming_configuration),
                 get_home() + "/.config");

  lookup.emplace(static_cast<u8>(Folders::app_menu),
                 get_home() + "/.local/share/applications");
  lookup.emplace(static_cast<u8>(Folders::fonts),
                 get_home() + "/.local/share/fonts");
  lookup.emplace(static_cast<u8>(Folders::data), get_home() + "/.local/share");

  lookup.emplace(static_cast<u8>(Folders::global_configuration), "/etc");
  lookup.emplace(static_cast<u8>(Folders::runtime), "/tmp");

  lookup.emplace(static_cast<u8>(Folders::executable_dir),
                 std::filesystem::current_path().string());

  auto file = std::ifstream(lookup[static_cast<u8>(Folders::home)] +
                            "/.config/user-dirs.dirs");
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (!line.contains('#')) {
        auto pair = line | std::ranges::views::split('=') |
                    std::ranges::to<std::vector<std::string>>();

        auto first = pair.at(0);
        auto pos = pair.at(1).find("$HOME");
        auto second =
            pair.at(1).replace(pos, 5, lookup[static_cast<u8>(Folders::home)]);
        second.erase(std::ranges::find(second, '"'));
        second.erase(std::ranges::find(second, '"'));

        if (first == "XDG_DESKTOP_DIR") {
          lookup.emplace(static_cast<u8>(Folders::desktop), second);
        }
        if (first == "XDG_DOWNLOAD_DIR") {
          lookup.emplace(static_cast<u8>(Folders::downloads), second);
        }
        if (first == "XDG_PUBLICSHARE_DIR") {
          lookup.emplace(static_cast<u8>(Folders::pub), second);
        }
        if (first == "XDG_DOCUMENTS_DIR") {
          lookup.emplace(static_cast<u8>(Folders::documents), second);
        }
        if (first == "XDG_MUSIC_DIR") {
          lookup.emplace(static_cast<u8>(Folders::music), second);
        }
        if (first == "XDG_PICTURES_DIR") {
          lookup.emplace(static_cast<u8>(Folders::pictures), second);
        }
        if (first == "XDG_VIDEOS_DIR") {
          lookup.emplace(static_cast<u8>(Folders::videos), second);
        }
      }
    }
  }
}

} // namespace

std::expected<std::filesystem::path, std::error_code>
linux_get_path(Folders fld) {
  if (lookup.empty()) {
    init();
    return lookup[static_cast<u8>(fld)];
  }
  return lookup[static_cast<u8>(fld)];
}
