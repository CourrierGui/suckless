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
    auto it = cfg.colors.begin();
    while (it != cfg.colors.end()) {
      _schemes.emplace_back(_drawable, *it++, *it++);
    }
    _drawable.setColorScheme(_schemes.front());
    _drawable.setFontSet(cfg.fonts);

    auto menu_height = _drawable.fontHeight() + 2;
    cfg.size.height = menu_height;

    if (cfg.size.width == 0)
      cfg.size.width = size.width;
    if (!cfg.topbar)
      cfg.pos.y = size.height - menu_height - cfg.pos.y;

    // match?
    XSetWindowAttributes swa;
    swa.override_redirect = true;
    swa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    swa.background_pixel = _schemes.front().background()->pixel;

    _menuWindow = XCreateWindow(
      d, root, cfg.pos.x, cfg.pos.y, cfg.size.width, cfg.size.height, 0,
      CopyFromParent, CopyFromParent, CopyFromParent,
      CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

    XClassHint ch = { "dmenu", "dmenu" };
    XSetClassHint(d, _menuWindow, &ch);

    XIM xim = XOpenIM(d, nullptr, nullptr, nullptr);
    if (!xim) {
      throw std::runtime_error("XOpenIM faled: counld not open input device");
    }
    XIC xic = XCreateIC(
      xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
      XNClientWindow, _menuWindow, XNFocusWindow, _menuWindow, NULL);

    XMapRaised(d, _menuWindow);
    // embed?
    _drawable.resize(cfg.size);

    draw(cfg);
  }

  void menu::draw(const config& cfg) {
    auto menu_height = _drawable.fontHeight() + 2;
    std::vector<std::string> text = {
      "Hello", "World", "Does", "It", "Works?"
    };
    std::string first = "First Of All";
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int w = 0;

    // clear menu
    _drawable.setColorScheme(_schemes[0]);
    _drawable.draw_rectangle({0, 0}, cfg.size, true, true);

    // draw prompt
    _drawable.setColorScheme(_schemes[1]);
    x = _drawable.draw_text({x, 0}, cfg.size, 5, ">", false);

    // draw input
    _drawable.setColorScheme(_schemes[0]);
    std::string input_text = "input text";
    unsigned int cursor_position = 4;
    auto cp = x
      + _drawable.textWidth(input_text)
      - _drawable.textWidth(input_text.substr(cursor_position));
    x = _drawable.draw_text(
      {x, 0}, cfg.size, 5, input_text, false);
    _drawable.draw_rectangle({cp, 2}, {2, cfg.size.height-4}, true, false);

    // draw closest match
    _drawable.setColorScheme(_schemes[2]);
    x = _drawable.draw_text({x, 0}, cfg.size, 5, "match", false);

    // draw other items
    _drawable.setColorScheme(_schemes[0]);
    for (const auto& s: text) {
      x = _drawable.draw_text({x, 0}, cfg.size, 20, s, false);
    }

    _drawable.map(_menuWindow, {0, 0}, {cfg.size.width, menu_height});
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
