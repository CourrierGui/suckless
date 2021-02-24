#pragma once

#include "draw.hpp"

#include <vector>

namespace dmenu {

  struct config {
    suckless::position pos;
    suckless::rect     size;
    bool               topbar;

    std::vector<std::string> fonts;
    std::string prompt;
    std::vector<std::string> colors;
    unsigned int lines;
    std::string delimiters;
  };

  class menu {
    public:
      static menu create(config& cfg);

    private:
      menu(Display* d, int screen, Window root, config& cfg, const suckless::rect& size);

      suckless::drawable _drawable;
      Window             _menuWindow;
  };

  void parse_args(int& argc, char* argv[], config& cfg);

} /* end of namespace dmenu */
