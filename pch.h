#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <range/v3/all.hpp>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#define DBG_FUNCSIG __FUNCSIG__
#include <Shobjidl.h>
#include <windows.h>
#elif __linux__
#define DBG_FUNCSIG __PRETTY_FUNCTION__
#include <sys/random.h>
#include <uuid.h>
#else
#error unsunsupported platform
#endif

#include "src/log.h"
#include "src/scoped_timer.h"
#include <fmt/format.h>
#include <sol/sol.hpp>
