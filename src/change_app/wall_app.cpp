#include "wall_app.h"

wallchanger::application::application(std::span<char *> args) {
  LOGGER_CREATE("changer");

  po::positional_options_description pos;
  pos.add("command", 1).add("subargs", -1);

  // Initialize All Commandgroups
  ranges::for_each(m_group_name, [&](auto &data) {
    m_group_vec.emplace_back(data, po::options_description(data));
  });

  // Loop To Add Appropriate Commandline Arguments To commandgroup
  for (auto &&[group_name, command_line] : m_cmds) {
    auto group = ranges::find(m_group_vec, group_name, &commandgroup::first);
    ranges::for_each(command_line, [&](auto &data) {
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
    fmt::print("\nAvaliable commands: {}\n", fmt::join(command_range, " , "));
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

  if (m_option_map.count("next") != 0U) {
    nlohmann::json obj = {};
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Get_Next_Wallpaper, obj));
  }
  if (m_option_map.count("previous") != 0U) {
    nlohmann::json obj = {};
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Get_Previous_Wallpaper, obj));
  }

  if (m_option_map.count("mark-favorate") != 0U) {
    nlohmann::json obj = {};
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Mark_Favorate, obj));
  }

  if (m_option_map.count("get-current") != 0U) {
    nlohmann::json obj = {};
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Get_Current, obj));
  }

  if (m_option_map.count("command") != 0U) {
    auto command = m_option_map["command"].as<std::string>();

    if (command == "collection") {
      m_collection_cmds();
    } else if (command == "configuration") {

      m_process_commands(CONFIG);

    } else if (command == "history") {

      m_process_commands(HISTORY);

    } else {
      fmt::print("{} command not supported\n", command);
    }
  }

  if (m_client.active()) {
    m_client.run();
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

  if (m_option_map.count("help") != 0U) {
    ranges::for_each(m_group_vec[sub_cmd].second.options(), [](auto data) {
      fmt::print("{1:<20} {0:^15} {2:<20}\n", "", data->format_name(),
                 data->description());
    });
  }
}

void wallchanger::application::m_collection_cmds() {
  m_process_commands(COLLECTION);
  if (m_option_map.count("create") != 0U) {
    auto res = m_option_map["create"].as<std::vector<std::string>>();
    nlohmann::json msg_new;
    msg_new["new_col_name"] = res[0];
    if (res.size() >= 2) {
      msg_new["col_empty"] = false;
      msg_new["col_path"] = res[1];
    } else {
      msg_new["col_path"] = {};
      msg_new["col_empty"] = true;
    }
    msg_new["recursive"] = false;
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Create_Collection, msg_new));
  }

  if (m_option_map.count("set-active") != 0U) {
    auto res = m_option_map["set-active"].as<std::string>();
    net::message<MessageType> msg;
    msg.header.id = MessageType::Change_Active_Collection;
    msg << res.c_str();
    m_client.send_message(msg);
  }

  if (m_option_map.count("list") != 0U) {
    auto cmd = m_option_map["list"].as<std::string>();
    nlohmann::json obj;
    if (!cmd.empty()) {
      obj["col"] = cmd;
      obj["col_only"] = false;
    } else {
      obj["col_only"] = true;
    }
    m_client.send_message(
        message_helper::json_to_msg(MessageType::List_Collections, obj));
  }

  if (m_option_map.count("add") != 0U) {
    auto res = m_option_map["add"].as<std::vector<std::string>>();
    nlohmann::json msg;
    msg["col_name"] = res.at(0);
    msg["wall"] = res.at(1);
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Add_To_Collection, msg));
  }

  if (m_option_map.count("remove") != 0U) {
    auto res = m_option_map["remove"].as<std::vector<std::string>>();
    nlohmann::json msg;
    if (res.size() > 1) {
      msg["col"] = res.at(0);
      msg["wall"] = res.at(1);
      msg["wall_only"] = true;
    } else {
      msg["col"] = res.at(0);
      msg["wall_only"] = false;
    }
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Remove, msg));
  }

  if (m_option_map.count("rename") != 0U) {
    auto res = m_option_map["rename"].as<std::vector<std::string>>();
    nlohmann::json msg;
    msg["col_name"] = res.at(0);
    msg["col_name_new"] = res.at(1);
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Rename_Collection, msg));
  }

  if (m_option_map.count("merge") != 0U) {
    auto res = m_option_map["merge"].as<std::vector<std::string>>();
    nlohmann::json msg;
    msg["col1"] = res.at(0);
    msg["col2"] = res.at(1);
    m_client.send_message(
        message_helper::json_to_msg(MessageType::Merge_Collection, msg));
  }

  if (m_option_map.count("move") != 0U) {
    auto res = m_option_map["move"].as<std::vector<std::string>>();
    nlohmann::json msg;
    msg["wall"] = res.at(0);
    msg["col_cur"] = res.at(1);
    msg["col_new"] = res.at(2);
    m_client.send_message(message_helper::json_to_msg(MessageType::Move, msg));
  }
}