#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <range/v3/all.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <Shobjidl.h>
#include <windows.h>
#elif __linux__
#include <sys/random.h>
#include <uuid.h>
#else
#error unsunsupported platform
#endif

#include "src/log.h"
#include "src/scoped_timer.h"
#include <fmt/format.h>
#include <sol/sol.hpp>
