#include "wall_app.hpp"
#include "../log.hpp"
#include "defines.hpp"
#include <helpers.hpp>
#include <print>
#include <ranges>
#include <tabulate/font_style.hpp>
#include <tabulate/table.hpp>

wallchanger::application::application(std::span<char *> args)
    : m_state("changer") {
  LOGGER_CREATE("changer");
  LOGGER_SET_FILE("changer", cache_directory() + "/logs/changer_log.txt");

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
    std::println(stdout, "program version: {}\nCompiler:{} {} {}",
                 WALLCHANGER_VERSION, COMPILER_NAME, COMPILER_VERSION,
                 COMPILER_ARCH);
  }

  if (m_option_map.contains("next")) {
  }
  if (m_option_map.contains("previous")) {
  }

  if (m_option_map.contains("mark-favorate")) {
  }

  if (m_option_map.contains("current-info")) {
  }

  if (m_option_map.contains("scrub")) {
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
      std::println("{} command not supported", command);
    }
  }

  m_state.store();

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
    if (res.size() >= 2) {
      if (!m_state.create_collection(res[0], res[1])) {
        return;
      }
    } else {
      if (!m_state.create_collection(res[0], {})) {
        return;
      }
    }
  }

  if (m_option_map.contains("activate")) {
    auto res = m_option_map["activate"].as<std::string>();
    if (!m_state.change_active(res)) {
      return;
    }
  }

  if (m_option_map.contains("list")) {
    auto res = m_option_map["list"].as<std::string>();
    if (res == "collections") {
      std::println("{:n}", m_state.list_collection());
    } else {
      tabulate::Table wall_table;
      wall_table.add_row({"No", "Wallpaper", "PathID", "State"});
      for (auto [idx, x] : std::views::enumerate(m_state.get_cache(res))) {
        wall_table.add_row({std::to_string(idx), x.cache_value,
                            std::to_string(x.loc),
                            std::format("{}", x.cache_state)});
      }
      for (usize i = 0; i < 4; i++) {
        wall_table[0][i]
            .format()
            .font_color(tabulate::Color::yellow)
            .font_align(tabulate::FontAlign::center)
            .font_style({tabulate::FontStyle::bold});
      }
      std::cout << wall_table << "\n";
    }
  }

  if (m_option_map.contains("add")) {
    auto res = m_option_map["add"].as<std::vector<std::string>>();
    if (res.size() == 2) {
      if (!m_state.add_to_collection(res[0], res[1])) {
        return;
      }
    }
  }

  if (m_option_map.contains("remove")) {
    auto res = m_option_map["remove"].as<std::vector<std::string>>();
    if (res.size() >= 2) {
      if (!m_state.remove_wallpaper(res[0], res[1])) {
        return;
      }
    } else {
      if (!m_state.remove_collection(res[0])) {
        return;
      }
    }
  }

  if (m_option_map.contains("rename")) {
    auto res = m_option_map["rename"].as<std::vector<std::string>>();
    if (res.size() == 2) {
      if (!m_state.rename_collection(res[0], res[1])) {
        return;
      }
    }
  }

  if (m_option_map.contains("merge")) {
    auto res = m_option_map["merge"].as<std::vector<std::string>>();
    if (res.size() == 2) {
      if (!m_state.merge_collection(res[0], res[1])) {
        return;
      }
    }
  }

  if (m_option_map.contains("move")) {
    auto res = m_option_map["move"].as<std::vector<std::string>>();
    if (res.size() == 3) {
      if (!m_state.move_wallpaper(res[1], res[2], res[0])) {
        return;
      }
    }
  }
}
