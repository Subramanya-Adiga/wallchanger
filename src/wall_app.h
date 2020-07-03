#pragma once
#include "../pch.h"
#include <boost/program_options.hpp>

namespace wallchanger {
class application
{
public:
  application(int argc, char *argv[]);
  int run();
};
} // namespace wallchanger
