#include "wall_app.h"

wallchanger::application::application(std::span<char *> args) {
  ranges::for_each(m_desc_name, [&](const std::string &name) {
    m_desc_vec.emplace_back(po::options_description(name));
  });
  m_add_options();
  po::store(po::parse_command_line(args.size(), args.data(),
                                   m_desc_vec[m_desc_pos::general]),
            m_option_map);
  po::notify(m_option_map);
}

int wallchanger::application::run() {
  if (m_option_map.count("help")) {
    std::cout << m_desc_vec[0] << "\n";
    return 1;
  }
  return 0;
}

void wallchanger::application::m_add_options() {
  m_desc_vec[m_desc_pos::general].add_options()("help", "produce help message");
}