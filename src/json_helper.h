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

  static void to_json(json &serialize_obj,
                      const wallchanger::cache_type_struct<key, value> &rhs) {
    serialize_obj = nlohmann::json{{"key", rhs.cache_key},
                                   {"value", rhs.cache_value},
                                   {"state", rhs.cache_state}};
  }

  static void from_json(const json &serialize_obj,
                        wallchanger::cache_type_struct<key, value> &rhs) {
    if (!serialize_obj.is_null()) {
      rhs.cache_key = serialize_obj.at("key");
      rhs.cache_value = serialize_obj.at("value");
      rhs.cache_state = serialize_obj.at("state");
    }
  }
};

template <typename key, typename value>
struct adl_serializer<wallchanger::cache<key, value>> {

  static void to_json(json &serialize_obj,
                      const wallchanger::cache<key, value> &rhs) {
    if (rhs.empty()) {
      serialize_obj = {};
    } else {
      auto arr = json::array();
      for (auto &&cache_obj : rhs) {
        arr.push_back(nlohmann::json{{"key", cache_obj.cache_key},
                                     {"value", cache_obj.cache_value},
                                     {"state", cache_obj.cache_state}});
      }
      serialize_obj = arr;
    }
  }

  static void from_json(const json &serialize_obj,
                        wallchanger::cache<key, value> &rhs) {
    if (!serialize_obj.is_null()) {
      rhs.clear();
      for (auto &&obj : serialize_obj) {
        rhs.insert(obj["key"].get<key>(), obj["value"].get<value>());
        rhs.set_state(obj["key"].get<key>(), obj["state"]);
      }
    }
  }
};

template <typename T> struct adl_serializer<wallchanger::cache_store_t<T>> {
  static void to_json(json &serialize_obj,
                      const wallchanger::cache_store_t<T> &rhs) {
    serialize_obj = nlohmann::json{
        {"Name", rhs.name}, {"Path", rhs.path}, {"Store", rhs.cache}};
  }

  static void from_json(const json &serialize_obj,
                        wallchanger::cache_store_t<T> &rhs) {
    if (!serialize_obj.is_null()) {
      rhs.name = serialize_obj.at("Name");
      rhs.path = serialize_obj.at("Path");
      rhs.cache = serialize_obj["Store"].get<T>();
    }
  }
};

} // namespace nlohmann
