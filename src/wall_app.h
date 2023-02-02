#pragma once
#include <boost/program_options.hpp>
#include <span>

namespace wallchanger {
namespace po = boost::program_options;

using commandgroup = std::pair<std::string, po::options_description>;
using commandline = std::vector<
    std::tuple<std::string_view, std::string_view, const po::value_semantic *>>;

class application {
  static constexpr std::string_view m_version = "0.1 pre alpha";
  static constexpr int m_group_size = 6;

  commandline m_general_options = {
      {"next,n", "next wallpaper", nullptr},
      {"previous,p", "previous wallpaper", nullptr},
      {"get-current", "print informatin about current wallpaper", nullptr},
      {"move-to-favorate", "move current wallpaper to favorate", nullptr},
      {"list-favorate", "list favorated wallpaper information", nullptr},
      {"version", "Program version", nullptr},
      {"help,h", "Print Help Message", nullptr},
      {"help-command", "command specific help", po::value<std::string>()}};

public:
  explicit application(std::span<char *> args);
  int run();

private:
  po::variables_map m_option_map;
  std::array<std::string, m_group_size> m_group_name = {
      "general", "history", "collection", "configuration"};

  std::vector<std::pair<std::string_view, commandline>> m_cmds = {
      {m_group_name[0], m_general_options}};
  std::vector<commandgroup> m_group_vec;
};
} // namespace wallchanger
