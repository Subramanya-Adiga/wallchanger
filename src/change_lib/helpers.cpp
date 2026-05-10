#include "helpers.hpp"

#include "known-folders.hpp"

std::string data_directory(){
    auto ret = get_path(Folders::data)->string();
    return ret + "/wallchanger";
}

std::string config_directory(){
    auto ret = get_path(Folders::local_configuration)->string();
    return ret + "/wallchanger";
}
