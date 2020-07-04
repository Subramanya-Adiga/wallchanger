#pragma once
#include "../pch.h"
#include <boost/program_options.hpp>
#include <span>

namespace wallchanger {
namespace po = boost::program_options;

class application {
  enum m_desc_pos { general, allowed, config, hidden };
  std::array<std::string, sizeof(m_desc_pos)> m_desc_name = {
      "General Options", "Allowed Option", "Configuration Option",
      "Hidden Option"};

public:
  explicit application(std::span<char *> args);
  int run();

private:
  po::variables_map m_option_map;
  std::vector<po::options_description> m_desc_vec;
  void m_add_options();
};
} // namespace wallchanger
