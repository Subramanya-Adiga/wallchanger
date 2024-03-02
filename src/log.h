#pragma once
#include <memory>
#include <range/v3/all.hpp>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string_view>

namespace debug::log {
class logger {
  std::vector<std::shared_ptr<spdlog::logger>> log_vec_;
  logger() = default;

public:
  void create_logger(std::string_view name) {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>(name.data(), sink);
    logger->set_level(spdlog::level::trace);
    log_vec_.emplace_back(logger);
  }
  void set_pattern(std::string_view name, std::string_view pattern) {
    auto rng_it = ranges::find_if(
        log_vec_, [&](auto &logger) { return (logger->name() == name); });
    if (rng_it != ranges::end(log_vec_)) {
      rng_it->get()->set_pattern(pattern.data());
    }
  }
  std::shared_ptr<spdlog::logger> get_logger(std::string_view name) {
    auto rng_it = ranges::find_if(
        log_vec_, [&](auto &logger) { return (logger->name() == name); });
    if (rng_it != ranges::end(log_vec_)) {
      return *rng_it;
    }
    return {};
  }
  void add_file_sink(std::string_view name, std::string_view file_name) {
    auto rng_it = ranges::find_if(
        log_vec_, [&](auto &logger) { return (logger->name() == name); });
    if (rng_it != ranges::end(log_vec_)) {
      auto file_sink =
          std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name.data());
      rng_it->get()->sinks().push_back(file_sink);
    }
  }
  static logger &get_instance() {
    static logger instance_;
    return instance_;
  }
};

#define LOGGER_CREATE(name)                                                    \
  debug::log::logger::get_instance().create_logger(name)
#define LOGGER_PATTERN(name, pattern)                                          \
  debug::log::logger::get_instance().set_pattern(name, pattern)
#define LOG_TRACE(name, ...)                                                   \
  debug::log::logger::get_instance().get_logger(name)->trace(__VA_ARGS__)
#define LOG_INFO(name, ...)                                                    \
  debug::log::logger::get_instance().get_logger(name)->info(__VA_ARGS__)
#define LOG_WARN(name, ...)                                                    \
  debug::log::logger::get_instance().get_logger(name)->warn(__VA_ARGS__)
#define LOG_ERR(name, ...)                                                     \
  debug::log::logger::get_instance().get_logger(name)->error(__VA_ARGS__)
#define LOG_CTER(name, ...)                                                    \
  debug::log::logger::get_instance().get_logger(name)->critical(__VA_ARGS__)
#define LOGGER_SET_FILE(name, file)                                            \
  debug::log::logger::get_instance().add_file_sink(name, file)
} // namespace debug::log
