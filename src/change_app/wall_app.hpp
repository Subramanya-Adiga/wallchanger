#pragma once
#include "wall_cmds.hpp"
#include <span>

namespace wallchanger {

class application {
  static constexpr std::string_view m_version = "0.1 pre alpha";
public:
  explicit application(std::span<char *> args);
  int run();

private:
  // no default constructor for po::parsed_options hence unique_ptr
  std::unique_ptr<po::parsed_options> m_parsed_options = nullptr;
  po::variables_map m_option_map;

  std::array<std::string, 4> m_group_name = {
      "global", "collection", "configuration", "history"};

  std::vector<std::pair<std::string_view, commandline>> m_cmds = {
      {m_group_name[subcommand_e::GLOBAL], global_options},
      {m_group_name[subcommand_e::COLLECTION], collection_options},
      {m_group_name[subcommand_e::CONFIG], config_options},
      {m_group_name[subcommand_e::HISTORY], history_options}};

  std::vector<commandgroup> m_group_vec;

  void m_process_commands(subcommand_e cmd);
  void m_collection_cmds();
};
} // namespace wallchanger
