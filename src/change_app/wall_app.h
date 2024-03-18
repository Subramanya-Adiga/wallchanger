#pragma once
#include "change_client.h"
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
      {"mark-favorate", "mark current wallpaper as Favorate", nullptr},
      {"version", "Program version", nullptr},
      {"ping", "ping server", nullptr},
      {"get-status", "get server status", nullptr},
      {"help,h", "Print Help Message", nullptr}};

  commandline m_collection_options{
      {"create", "create new collection. [Col] [Path]",
       po::value<std::vector<std::string>>()->multitoken()},
      {"set-active", "Change Active Collection To [Arg].",
       po::value<std::string>()},
      {"add", "add wallpaper to collection. [Col] [Wall]",
       po::value<std::vector<std::string>>()->multitoken()},
      {"remove,r",
       "remove collection or wallpaper from collection. [Col] or [Col] [wall]",
       po::value<std::vector<std::string>>()->multitoken()},
      {"rename", "rename collection. [Col] [Arg]",
       po::value<std::vector<std::string>>()->multitoken()},
      {"merge", "merge two collection. [col1] [col2]",
       po::value<std::vector<std::string>>()->multitoken()},
      {"list,l",
       "list wallpapers in collection [Col]. If Col Is Empty List All "
       "Collections.",
       po::value<std::string>()},
      {"move,m",
       "move wallpaper from one to another [wall] [col1] [col2](unimplemented)",
       po::value<std::vector<std::string>>()->multitoken()},
      {"help,h", "print help message", nullptr}};

  commandline m_config_options{
      {"collect-favorates",
       "collect favorates in all collections to new favorate collection",
       nullptr},
      {"recursive-dir",
       "recursively search directories when adding to a collection.[default = "
       "false]",
       nullptr},
      {"priority-favorate", "prioratize favorate collection first", nullptr},
      {"interval",
       "set change interval between wallpapers in hours.[default = 24]",
       po::value<int>()},
      {"help,h", "print help message", nullptr}};

  commandline m_history_options{
      {"reset-state", "reset state in all collections", nullptr},
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
      {m_group_name[COLLECTION], m_collection_options},
      {m_group_name[CONFIG], m_config_options},
      {m_group_name[HISTORY], m_history_options}};

  std::vector<commandgroup> m_group_vec;
  wallchanger::change_client m_client;

  void m_process_commands(subcommand_e cmd);
  void m_collection_cmds();
};
} // namespace wallchanger
