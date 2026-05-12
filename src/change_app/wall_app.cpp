#include "wall_app.hpp"
#include "../log.hpp"
#include <print>

wallchanger::application::application(std::span<char *> args) {
  LOGGER_CREATE("changer");

  po::positional_options_description pos;
  pos.add("command", 1).add("subargs", -1);

  // Initialize All CommandGroups
  std::ranges::for_each(m_group_name, [this](auto &data) {
    m_group_vec.emplace_back(data, po::options_description(data));
  });

  // Loop To Add Appropriate Commandline Arguments To commandgroup
  for (auto &&[group_name, command_line] : m_cmds) {
    auto group =
        std::ranges::find(m_group_vec, group_name, &commandgroup::first);

    std::ranges::for_each(command_line, [group](auto &data) {
      if (auto &&[cmd, desc, value] = data; value != nullptr) {
        group->second.add_options()(cmd.c_str(), value, desc.c_str());
      } else {
        group->second.add_options()(cmd.c_str(), desc.c_str());
      }
    });
  }

  m_parsed_options = std::make_unique<po::parsed_options>(
      po::command_line_parser(static_cast<int>(args.size()), args.data())
          .options(m_group_vec.front().second)
          .positional(pos)
          .allow_unregistered()
          .run());

  po::store(*m_parsed_options, m_option_map);
}

int wallchanger::application::run() {

  if ((!m_option_map.contains("command")) && m_option_map.contains("help")) {
    std::println("\nusage: wallchanger [command] [argument]\n");

    auto commandline_range = std::ranges::views::drop(
        m_group_vec[subcommand_e::GLOBAL].second.options(), 2);

    std::ranges::for_each(commandline_range, [](auto &data) {
      std::println("{1:<20} {0:^15} {2:<20}", "", data->format_name(),
                   data->description());
    });

    std::println("\nAvaliable commands: {:n}",
                 std::ranges::views::drop(m_group_name, 1));
  }

  if (m_option_map.contains("version")) {
    std::println(stdout, "program version: {}", m_version);
  }

  if (m_option_map.contains("next")) {
  }
  if (m_option_map.contains("previous")) {
  }

  if (m_option_map.contains("mark-favorate")) {
  }

  if (m_option_map.contains("get-current")) {
  }

  if (m_option_map.contains("command")) {
    auto command = m_option_map["command"].as<std::string>();

    if (command == "collection") {
      m_collection_cmds();
    } else if (command == "configuration") {

      m_process_commands(subcommand_e::CONFIG);

    } else if (command == "history") {

      m_process_commands(subcommand_e::HISTORY);

    } else {
      std::print("{} command not supported\n", command);
    }
  }

  return 0;
}

void wallchanger::application::m_process_commands(subcommand_e sub_cmd) {
  auto options = po::collect_unrecognized(m_parsed_options->options,
                                          po::include_positional);
  options.erase(options.begin());
  po::store(po::command_line_parser(options)
                .options(m_group_vec[sub_cmd].second)
                .run(),
            m_option_map);

  if (m_option_map.contains("help")) {
    std::ranges::for_each(
        m_group_vec[sub_cmd].second.options(), [](auto &&data) {
          std::println("{1:<20} {0:^15} {2:<20}", "", data->format_name(),
                       data->description());
        });
  }
}

void wallchanger::application::m_collection_cmds() {
  m_process_commands(subcommand_e::COLLECTION);
  if (m_option_map.contains("create")) {
    auto res = m_option_map["create"].as<std::vector<std::string>>();
  }

  if (m_option_map.contains("set-active")) {
    auto res = m_option_map["set-active"].as<std::string>();
  }

  if (m_option_map.contains("list")) {
    auto res = m_option_map["list"].as<std::string>();
  }

  if (m_option_map.contains("add")) {
    auto res = m_option_map["add"].as<std::vector<std::string>>();
  }

  if (m_option_map.contains("remove")) {
    auto res = m_option_map["remove"].as<std::vector<std::string>>();
  }

  if (m_option_map.contains("rename")) {
    auto res = m_option_map["rename"].as<std::vector<std::string>>();
  }

  if (m_option_map.contains("merge")) {
    auto res = m_option_map["merge"].as<std::vector<std::string>>();
  }

  if (m_option_map.contains("move")) {
    auto res = m_option_map["move"].as<std::vector<std::string>>();
  }
}
