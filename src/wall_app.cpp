#include "wall_app.h"

wallchanger::application::application(std::span<char *> args) {
  ranges::for_each(m_desc_name, [&](auto &data) {
    m_cmd_vec.emplace_back(0, data, po::options_description(data));
  });
  auto ret = ranges::find(m_cmd_vec, m_desc_name[0], &commandline::cmd_name);
  ranges::for_each(m_general_options, [&](auto &data) {
    ret->cmd.add_options()(data.first.c_str(), data.second.c_str());
  });

  po::store(
      po::parse_command_line(args.size(), args.data(), m_cmd_vec.front().cmd),
      m_option_map);
  po::notify(m_option_map);
}

int wallchanger::application::run() {
  if (m_option_map.count("help") != 0U) {
    std::cout << m_cmd_vec[0].cmd << "\n";
  }
  return 0;
}