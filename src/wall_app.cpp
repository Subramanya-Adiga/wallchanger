#include "wall_app.h"
#include "log.h"

wallchanger::application::application(std::span<char *> args) {
  LOGGER_CREATE("changer");
  ranges::for_each(m_group_name, [&](auto &data) {
    m_group_vec.emplace_back(data, po::options_description(data));
  });
  for (auto &&[f, s] : m_cmds) {
    auto group = ranges::find(m_group_vec, f, &commandgroup::first);
    ranges::for_each(s, [&](auto &data) {
      if (std::get<2>(data) != nullptr) {
        group->second.add_options()(std::get<0>(data).data(), std::get<2>(data),
                                    std::get<1>(data).data());
      } else {
        group->second.add_options()(std::get<0>(data).data(),
                                    std::get<1>(data).data());
      }
    });
  }
  po::store(po::parse_command_line(static_cast<int>(args.size()), args.data(),
                                   m_group_vec.front().second),
            m_option_map);
  po::notify(m_option_map);
}

int wallchanger::application::run() {
  using namespace std::literals;
  if (m_option_map.count("help") != 0U) {
    fmt::print("\nusage: wallchanger [command] [option]\n\n"sv);
    ranges::for_each(m_group_vec[0].second.options(), [](auto data) {
      fmt::print("{1:<20} {0:^15} {2:<20}\n"sv, "", data->format_name(),
                 data->description());
    });
  }

  if (m_option_map.count("version") != 0U) {
    fmt::print(stdout, "program version: {}\n", m_version);
  }

  if (m_option_map.count("help-command") != 0U) {
    const auto &cmd = m_option_map["help-command"].as<std::string>();
    if (cmd == "history") {
      fmt::print("\nusage: wallchanger [command] [option]\n\n"sv);
      ranges::for_each(m_group_vec[1].second.options(), [](auto data) {
        fmt::print("{1:<20} {0:^15} {2:<20}\n"sv, "", data->format_name(),
                   data->description());
      });
    } else if (cmd == "collection") {
      fmt::print("\nusage: wallchanger [command] [option]\n\n"sv);
      ranges::for_each(m_group_vec[2].second.options(), [](auto data) {
        fmt::print("{1:<20} {0:^15} {2:<20}\n"sv, "", data->format_name(),
                   data->description());
      });
    } else {
      fmt::print("{} is not a supported command\n"sv, cmd);
    }
  }

  return 0;
}