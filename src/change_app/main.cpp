#include "wall_app.hpp"

int main(int argc, char *argv[]) {
  wallchanger::application app({argv, static_cast<unsigned long>(argc)});
  return app.run();
}
