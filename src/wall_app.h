#pragma once
#include "../pch.h"
#include "range/v3/algorithm/for_each.hpp"
#include <boost/program_options.hpp>
#include <span>

namespace wallchanger {
namespace po = boost::program_options;

struct commandline {
  int cmd_id;
  std::string cmd_name;
  po::options_description cmd;
};

class application {
  enum m_desc_pos { general, allowed, config, hidden };
  std::array<std::string, sizeof(m_desc_pos)> m_desc_name = {
      "General Options", "Commands", "Configuration Option", "Hidden Option"};
  std::vector<std::pair<std::string, std::string>> m_general_options = {
      {"next,n", "next wallpaper"},
      {"previous,p", "previous wallpaper"},
      {"history,h", "print wallpaper history"},
      {"get-current", "print current wallpaper name and location"},
      {"set-wallpaper",
       "set file as wallpaper or file at location as wallpaper"},
      {"move-to-favoriate", "move current wallpaper to favoriate"},
      {"version", "Program version"},
      {"help,h", "Print Help Message"}};

public:
  explicit application(std::span<char *> args);
  int run();

private:
  po::variables_map m_option_map;
  std::vector<commandline> m_cmd_vec;
  void m_add_options();
};
} // namespace wallchanger
