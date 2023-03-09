#include "wall_app.h"
#include "log.h"
#include <fmt/chrono.h>

wallchanger::application::application(std::span<char *> args) {
  LOGGER_CREATE("changer");

  po::positional_options_description pos;
  pos.add("command", 1).add("subargs", -1);

  // Initialize All Commandgroups
  ranges::for_each(m_group_name, [&](auto &data) {
    m_group_vec.emplace_back(data, po::options_description(data));
  });

  // Loop To Add Appropriate Commandline Arguments To commandgroup
  for (auto &&[group_name, commandline] : m_cmds) {
    auto group = ranges::find(m_group_vec, group_name, &commandgroup::first);
    ranges::for_each(commandline, [&](auto &data) {
      if (std::get<value>(data) != nullptr) {
        group->second.add_options()(std::get<cmd>(data).data(),
                                    std::get<value>(data),
                                    std::get<desc>(data).data());
      } else {
        group->second.add_options()(std::get<cmd>(data).data(),
                                    std::get<desc>(data).data());
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
  m_connected = m_client.connect("127.0.0.1", 60000);
}

int wallchanger::application::run() {

  if ((m_option_map.count("help") != 0U) &&
      (m_option_map.count("command") == 0U)) {
    fmt::print("\nusage: wallchanger [command] [argument]\n\n");
    // subrange to exclude first 2 options
    auto commandline_range =
        ranges::make_subrange(m_group_vec[GLOBAL].second.options().begin() + 2,
                              m_group_vec[GLOBAL].second.options().end());

    ranges::for_each(commandline_range, [](auto data) {
      fmt::print("{1:<20} {0:^15} {2:<20}\n", "", data->format_name(),
                 data->description());
    });
    // subrange to exclude first subcommand
    auto command_range =
        ranges::make_subrange(m_group_name.begin() + 1, m_group_name.end());
    fmt::print("\nAvaliable commands: {}", fmt::join(command_range, " , "));
  }

  if (m_option_map.count("version") != 0U) {
    fmt::print(stdout, "program version: {}\n", m_version);
  }

  if (m_option_map.count("ping") != 0U) {
    m_client.ping_server();
  }

  if (m_option_map.count("get-status") != 0U) {
    m_client.get_server_status();
  }

  if (m_option_map.count("command") != 0U) {
    auto command = m_option_map["command"].as<std::string>();

    if (command == "collection") {

      m_process_commands(COLLECTION);

    } else if (command == "configuration") {

      m_process_commands(CONFIG);

    } else if (command == "history") {

      m_process_commands(HISTORY);

    } else {
      fmt::print("{} command not supported\n", command);
    }
  }
  while (!m_stop_processing) {
    m_process_server_commands();
  }
  return 0;
}

void wallchanger::application::m_process_commands(subcommand_e cmd) {
  auto options = po::collect_unrecognized(m_parsed_options->options,
                                          po::include_positional);
  options.erase(options.begin());
  po::store(
      po::command_line_parser(options).options(m_group_vec[cmd].second).run(),
      m_option_map);

  if (m_option_map.count("help") != 0U) {
    ranges::for_each(m_group_vec[cmd].second.options(), [](auto data) {
      fmt::print("{1:<20} {0:^15} {2:<20}\n", "", data->format_name(),
                 data->description());
    });
  }
}

void wallchanger::application::m_process_server_commands() {
  if (m_client.is_connected()) {
    if (!m_client.incomming().empty()) {
      auto msg = m_client.incomming().pop_front().msg;
      switch (msg.header.id) {
      case MessageType::Server_GetStatus: {
        std::vector<uint8_t> res;
        res.resize(msg.body.size());
        std::memcpy(res.data(), msg.body.data(), msg.body.size());
        std::cout << nlohmann::json::from_cbor(res) << "\n";
        m_stop_processing = true;
        break;
      }
      case MessageType::Server_GetPing: {
        auto time_now = std::chrono::system_clock::now();
        auto time_then = msg.time;
        fmt::print("{}\n", std::chrono::duration_cast<std::chrono::seconds>(
                               time_now - time_then));
        m_stop_processing = true;
        break;
      }
      case MessageType::Client_Accepted:
      case MessageType::Client_AssignID:
      case MessageType::Client_RegisterWithServer:
      case MessageType::Client_UnregisterWithServer:
        break;
      }
    }
  }
}