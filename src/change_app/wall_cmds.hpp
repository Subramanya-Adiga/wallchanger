#pragma once
#include <boost/program_options.hpp>
#include <defines.hpp>

namespace wallchanger {

namespace po = boost::program_options;

struct command_desc {
  std::string command;
  std::string description;
  const po::value_semantic *value;
};

using commandgroup = std::pair<std::string, po::options_description>;
using commandline = std::vector<command_desc>;

enum subcommand_e : u8 { GLOBAL, COLLECTION, CONFIG, HISTORY };
enum command_loc : u8 { cmd, desc, value };

static const commandline global_options = {
    {.command = "command",
     .description = "command to execute",
     .value = po::value<std::string>()},
    {.command = "subargs",
     .description = "arguments to command",
     .value = po::value<std::vector<std::string>>()},
    {.command = "next,n", .description = "next wallpaper", .value = nullptr},
    {.command = "previous,p",
     .description = "previous wallpaper",
     .value = nullptr},
    {.command = "get-current",
     .description = "print informatin about current wallpaper",
     .value = nullptr},
    {.command = "mark-favorate",
     .description = "mark current wallpaper as Favorate",
     .value = nullptr},
    {.command = "version", .description = "Program version", .value = nullptr},
    {.command = "help,h",
     .description = "Print Help Message",
     .value = nullptr}};

static const commandline collection_options{
    {.command = "create",
     .description = "create new collection. [Col] [Path]",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "set-active",
     .description = "Change Active Collection To [Arg].",
     .value = po::value<std::string>()},
    {.command = "add",
     .description = "add wallpaper to collection. [Col] [Wall]",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "remove,r",
     .description = "remove collection or wallpaper from collection. [Col] "
                    "or [Col] [wall]",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "rename",
     .description = "rename collection. [Col] [Arg]",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "merge",
     .description = "merge two collection. [col1] [col2]",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "list,l",
     .description =
         "list wallpapers in collection [Col]. If Col Is Empty List All "
         "Collections.",
     .value = po::value<std::string>()},
    {.command = "move,m",
     .description = "move wallpaper from one to another [wall] [col1] "
                    "[col2](unimplemented)",
     .value = po::value<std::vector<std::string>>()->multitoken()},
    {.command = "help,h",
     .description = "print help message",
     .value = nullptr}};

static const commandline config_options{
    {.command = "collect-favorates",
     .description =
         "collect favorates in all collections to new favorate collection",
     .value = nullptr},
    {.command = "recursive-dir",
     .description = "recursively search directories when adding to a "
                    "collection.[default = "
                    "false]",
     .value = nullptr},
    {.command = "priority-favorate",
     .description = "prioratize favorate collection first",
     .value = nullptr},
    {.command = "interval",
     .description =
         "set change interval between wallpapers in hours.[default = 24]",
     .value = po::value<int>()},
    {.command = "help,h",
     .description = "print help message",
     .value = nullptr}};

static const commandline history_options{{.command = "reset-state",
                             .description = "reset state in all collections",
                             .value = nullptr},
                            {.command = "help,h",
                             .description = "print help message",
                             .value = nullptr}};
} // namespace wallchanger
