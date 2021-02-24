#include "dmenu.hpp"
#include "draw.hpp"

#include <iostream>
#include <stdexcept>

namespace dmenu {

  menu menu::create(config& cfg) {
    Display* d = XOpenDisplay(nullptr);
    if (!d) {
      throw std::runtime_error("Cannot open display.");
    }
    auto screen = DefaultScreen(d);
    auto root   = RootWindow(d, screen);

    XWindowAttributes wa;
    XGetWindowAttributes(d, root, &wa);

    suckless::rect s = {
      static_cast<unsigned int>(wa.width),
      static_cast<unsigned int>(wa.height)
    };
    return {d, screen, root, cfg, s};
  }

  menu::menu(
    Display* d, int screen, Window root, config& cfg, const suckless::rect& size) :
    _drawable(d, screen, root, size), _menuWindow{}
  {
    _drawable.setFontSet(cfg.fonts);

    XSetWindowAttributes swa;
    swa.override_redirect = true;
    swa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;


    auto menu_height = _drawable.fontHeight() + 2;
    cfg.size.height = menu_height;

    if (cfg.size.width == 0)
      cfg.size.width = size.width;
    if (!cfg.topbar)
      cfg.pos.y = size.height - menu_height - cfg.pos.y;

    _menuWindow = XCreateWindow(
      d, root, cfg.pos.x, cfg.pos.y, cfg.size.width, cfg.size.height, 0,
      CopyFromParent, CopyFromParent, CopyFromParent,
      CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

    XClassHint ch = { "dmenu", "dmenu" };
    XSetClassHint(d, _menuWindow, &ch);
    XMapRaised(d, _menuWindow);

    _drawable.setColorScheme(suckless::color_scheme(_drawable, cfg.colors));
    _drawable.resize(cfg.size);

    _drawable.draw_rectangle({0, 0}, cfg.size, true, false);
    _drawable.draw_text({0, menu_height-2}, cfg.size, 2, "Hello, World!", false);
    _drawable.map(_menuWindow, {0, 0}, cfg.size);
  }

  void parse_args(int& argc, char* argv[], config& cfg) {
    for (int i=0; i<argc; ++i) {
      char* arg = argv[i];
      if (!strcmp(arg, "-x")) {
        cfg.pos.x = atoi(argv[++i]);
      } else if (!strcmp(arg, "-y")) {
        cfg.pos.y = atoi(argv[++i]);
      } else if (!strcmp(arg, "-z")) {
        cfg.size.width = atoi(argv[++i]);
      } else if (!strcmp(arg, "-b")) {
        cfg.topbar = false;
      } else {
      }
    }
  }

} /* end of namespace dmenu */
