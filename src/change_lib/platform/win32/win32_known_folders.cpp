#include "win32_known_folders.hpp"

#include "../../known-folders.hpp"
#include <shlobj.h>
#include <strsafe.h>

namespace {
KNOWNFOLDERID convert(Folders fld) {
  switch (fld) {
  case Folders::home:
    return FOLDERID_Profile;
  case Folders::documents:
    return FOLDERID_Documents;
  case Folders::pictures:
    return FOLDERID_Pictures;
  case Folders::music:
    return FOLDERID_Music;
  case Folders::videos:
    return FOLDERID_Videos;
  case Folders::desktop:
    return FOLDERID_Desktop;
  case Folders::downloads:
    return FOLDERID_Downloads;
  case Folders::pub:
    return FOLDERID_Public;
  case Folders::fonts:
    return FOLDERID_Fonts;
  case Folders::app_menu:
    return FOLDERID_StartMenu;
  case Folders::roaming_configuration:
    return FOLDERID_RoamingAppData;
  case Folders::local_configuration:
    return FOLDERID_LocalAppData;
  case Folders::global_configuration:
    return FOLDERID_ProgramData;
  case Folders::data:
  case Folders::cache:
  case Folders::runtime:
    return FOLDERID_LocalAppData;
  default:
    return {};
  }
}
} // namespace

std::expected<std::filesystem::path, std::error_code>
get_win32_path(Folders fld) {
  if (fld == Folders::executable_dir) {
    wchar_t file_name[MAX_PATH] = {};
    auto res = GetModuleFileName(nullptr, file_name, MAX_PATH);
    if (res == 0) {
      return std::unexpected{make_win32_error_code()};
    }
    return std::filesystem::path{to_utf8(file_name, res + 1)}.parent_path();
  }

  auto val = convert(fld);

  PWSTR path = nullptr;
  auto res = SHGetKnownFolderPath(val, 0, nullptr, &path);
  if (res != S_OK) {
    CoTaskMemFree(path);
    return std::unexpected{make_HRESULT_error_code(res)};
  }

  usize len = 0;
  auto str_res = StringCchLength(path, STRSAFE_MAX_CCH, &len);
  if (str_res != S_OK) {
    CoTaskMemFree(path);
    return std::unexpected{make_HRESULT_error_code(str_res)};
  }

  auto str = to_utf8(path, static_cast<u32>(len));
  CoTaskMemFree(path);
  if (fld == Folders::cache || fld == Folders::data ||
      fld == Folders::runtime) {
    return str + "\\Temp\\";
  }
  return str;
}
