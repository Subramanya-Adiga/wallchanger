#pragma once
#include "defines.hpp"
#include <expected>
#include <filesystem>
#include <system_error>

enum class Folders : u8 {
  // Windows default: `%SystemDrive%\Users\%USERNAME%`
  home,
  // Windows default: `%USERPROFILE%\Documents`
  //   XDG directory: `XDG_DOCUMENTS_DIR`
  documents,
  // Windows default: `%USERPROFILE%\Pictures`
  //   XDG directory: `XDG_PICTURES_DIR`
  pictures,
  // Windows default: `%USERPROFILE%\Music`
  //   XDG directory: `XDG_MUSIC_DIR`
  music,
  // Windows default: `%USERPROFILE%\Videos`
  //   XDG directory: `XDG_VIDEOS_DIR`
  videos,
  // Windows default: `%USERPROFILE%\Desktop`
  //   XDG directory: `XDG_DESKTOP_DIR`
  desktop,
  // Windows default: `%USERPROFILE%\Downloads`
  //   XDG directory: `XDG_DOWNLOAD_DIR`
  downloads,
  // Windows default: `%PUBLIC%` (`%SystemDrive%\Users\Public`)
  //   XDG directory: `XDG_PUBLICSHARE_DIR`
  pub,
  // Windows default: `%windir%\Fonts`
  //   XDG directory: `XDG_DATA_HOME/fonts`
  fonts,
  // Windows default: `%APPDATA%\Microsoft\Windows\Start Menu`
  //   XDG directory: `XDG_DATA_HOME/applications`
  app_menu,
  // The base directory relative to which user-specific non-essential (cached)
  // data should be written.
  // Windows:       `%LOCALAPPDATA%\Temp`
  //   XDG directory: `XDG_CACHE_HOME`
  cache,
  // The base directory relative to which user-specific configuration files
  // should be written.
  // Windows default: %APPDATA% (%USERPROFILE%\AppData\Roaming)
  //   XDG directory: `XDG_CONFIG_HOME`
  roaming_configuration,
  // The base directory relative to which user-specific configuration files
  // should be written.
  // Windows default: `%LOCALAPPDATA%` (`%USERPROFILE%\AppData\Local`)
  //   XDG directory: `XDG_CONFIG_HOME`
  local_configuration,
  // The base directory relative to which global configuration files should be
  // searched.
  // Windows default: `%ALLUSERSPROFILE%`
  // (`%ProgramData%`,`%SystemDrive%\ProgramData`)
  //   XDG directory: `XDG_CONFIG_DIRS` (first directory)
  global_configuration,
  // The base directory relative to which user-specific data files should be
  // written.
  // Windows:       `%LOCALAPPDATA%\Temp`
  // XDG directory: `XDG_DATA_HOME`
  // XDG's definition of `XDG_DATA_HOME`: There is a set of preference ordered
  // base directories
  data,
  // The base directory relative to which user-specific runtime files and other
  // file objects should be placed.
  // Windows:       `%LOCALAPPDATA%\Temp`
  // XDG directory: `XDG_RUNTIME_DIR`
  runtime,
  // Get the directory that contains the current executable.
  executable_dir,
};

std::expected<std::filesystem::path, std::error_code> get_path(Folders folder);
