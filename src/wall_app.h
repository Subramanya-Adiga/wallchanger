#pragma once
#include <boost/program_options.hpp>
#include <span>

namespace wallchanger {
namespace po = boost::program_options;

class application {
  static constexpr std::string_view m_version = "0.1 pre alpha";
  static constexpr int m_group_size = 4;

  using commandgroup = std::pair<std::string, po::options_description>;
  using commandline = std::vector<std::tuple<std::string_view, std::string_view,
                                             const po::value_semantic *>>;
  enum subcommand_e { GLOBAL, COLLECTION, CONFIG, HISTORY };
  enum command_loc { cmd, desc, value };

  commandline m_global_options = {
      {"command", "command to execute", po::value<std::string>()},
      {"subargs", "arguments to command",
       po::value<std::vector<std::string>>()},
      {"next,n", "next wallpaper", nullptr},
      {"previous,p", "previous wallpaper", nullptr},
      {"get-current", "print informatin about current wallpaper", nullptr},
      {"move-to-favorate", "move current wallpaper to favorate", nullptr},
      {"list-favorate", "list favorated wallpaper information", nullptr},
      {"version", "Program version", nullptr},
      {"help,h", "Print Help Message", nullptr}};

  commandline m_collection_options{
      {"create", "create new collection", po::value<std::string>()},
      {"help,h", "print help message", nullptr}};

public:
  explicit application(std::span<char *> args);
  int run();

private:
  // no default constructor for po::parsed_options hence unique_ptr
  std::unique_ptr<po::parsed_options> m_parsed_options = nullptr;
  po::variables_map m_option_map;

  std::array<std::string, m_group_size> m_group_name = {
      "global", "collection", "configuration", "history"};

  std::vector<std::pair<std::string_view, commandline>> m_cmds = {
      {m_group_name[GLOBAL], m_global_options},
      {m_group_name[COLLECTION], m_collection_options}};

  std::vector<commandgroup> m_group_vec;
};
} // namespace wallchanger
