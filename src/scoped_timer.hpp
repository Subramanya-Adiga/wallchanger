#pragma once
#include "log.hpp"
#include <chrono>
#include <string_view>

namespace debug::timer {
template <typename to_dur> class scoped_timer {
  using timer = std::chrono::time_point<std::chrono::steady_clock>;
  timer start_;
  timer end_;
  std::string_view name_;
  std::string_view ext_;

public:
  scoped_timer() = delete;
  explicit scoped_timer(std::string_view name, std::string_view ext) {
    name_ = name;
    ext_ = ext;
    start_ = std::chrono::steady_clock::now();
    LOGGER_CREATE("Timer");
    LOGGER_PATTERN("Timer", "[%c][%n] %v%$");
  }
  ~scoped_timer() {
    end_ = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<to_dur>(end_ - start_).count();
    LOG_INFO("Timer", "{0} {1} {2}", name_, elapsed, ext_);
  }
};
#define TIMER_NAME_NANO(name)                                                  \
  debug::timer::scoped_timer<std::chrono::nanoseconds> timer##__LINE__(name,   \
                                                                       "ns")
#define TIMER_NANO() TIMER_NAME_NANO(DBG_FUNCSIG)
#define TIMER_NAME_MICRO(name)                                                 \
  debug::timer::scoped_timer<std::chrono::microseconds> timer##__LINE__(name,  \
                                                                        "μs")
#define TIMER_MICRO() TIMER_NAME_MICRO(DBG_FUNCSIG)
#define TIMER_NAME_MILLI(name)                                                 \
  debug::timer::scoped_timer<std::chrono::milliseconds> timer##__LINE__(name,  \
                                                                        "ms")
#define TIMER_MILLI() TIMER_NAME_MILLI(DBG_FUNCSIG)
#define TIMER_NAME_SEC(name)                                                   \
  debug::timer::scoped_timer<std::chrono::seconds> timer##__LINE__(name, "s")
#define TIMER_SEC() TIMER_NAME_SEC(DBG_FUNCSIG)
} // namespace debug::timer
