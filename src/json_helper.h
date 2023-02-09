#pragma once
#include "wall_cache.h"
#include "wall_cache_library.h"
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>

NLOHMANN_JSON_SERIALIZE_ENUM(wallchanger::cache_state_e,
                             {
                                 {wallchanger::cache_state_e::null, nullptr},
                                 {wallchanger::cache_state_e::unused, "unused"},
                                 {wallchanger::cache_state_e::used, "used"},
                                 {wallchanger::cache_state_e::favorate,
                                  "favorate"},
                             });

namespace nlohmann {

template <typename key, typename value>
struct adl_serializer<wallchanger::cache_type_struct<key, value>> {

  static void to_json(json &j,
                      const wallchanger::cache_type_struct<key, value> &rhs) {
    j = nlohmann::json{{"key", rhs.cache_key},
                       {"value", rhs.cache_value},
                       {"state", rhs.cache_state}};
  }

  static void from_json(const json &j,
                        wallchanger::cache_type_struct<key, value> &rhs) {
    if (!j.is_null()) {
      rhs.cache_key = j.at("key");
      rhs.cache_value = j.at("value");
      rhs.cache_state = j.at("state");
    }
  }
};

template <typename key, typename value>
struct adl_serializer<wallchanger::cache<key, value>> {

  static void to_json(json &j, const wallchanger::cache<key, value> &rhs) {
    if (rhs.empty()) {
      j = {};
    } else {
      auto arr = json::array();
      for (auto &&x : ranges::make_subrange(rhs.cbegin(), rhs.cend())) {
        arr.push_back(nlohmann::json{{"key", x.cache_key},
                                     {"value", x.cache_value},
                                     {"state", x.cache_state}});
      }
      j = arr;
    }
  }

  static void from_json(const json &j, wallchanger::cache<key, value> &rhs) {
    if (!j.is_null()) {
      rhs.clear();
      for (auto &&obj : j) {
        rhs.insert(obj["key"].get<key>(), obj["value"].get<value>());
        rhs.set_state(obj["key"].get<key>(), obj["state"]);
      }
    }
  }
};

template <typename T> struct adl_serializer<wallchanger::cache_store_t<T>> {
  static void to_json(json &j, const wallchanger::cache_store_t<T> &rhs) {
    j = nlohmann::json{
        {"Name", rhs.name}, {"Path", rhs.path}, {"Store", rhs.cache}};
  }
  static void from_json(const json &j, wallchanger::cache_store_t<T> &rhs) {
    if (!j.is_null()) {
      rhs.name = j.at("Name");
      rhs.path = j.at("Path");
      rhs.cache = j["Store"].get<T>();
    }
  }
};

} // namespace nlohmann