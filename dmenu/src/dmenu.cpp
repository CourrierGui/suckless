#include "dmenu.hpp"
#include "draw.hpp"

#include <chrono>
#include <thread>
#include <stdexcept>

#include <iostream>

namespace dmenu {

  auto openDisplay() -> Display* {
    auto d = XOpenDisplay(nullptr);
    if (!d) {
      std::cerr << "Could not open display.\n";
      std::exit(1);
    }
    return d;
  }

  Items::Items(std::vector<std::string>&& items) :
    _items{std::move(items)} {  }

  Items Items::readStdin() {
    std::string buffer;
    std::vector<std::string> items;

    while (std::getline(std::cin, buffer)) {
      items.push_back(buffer);
      buffer.clear();
    }
    return Items(std::move(items));
  }

  auto Items::items() const -> const std::vector<std::string>& {
    return _items;
  }

  void match(const std::string& input) {

  }

  Keyboard::Keyboard(Display* display) : _text{}, _ic{} {
    _grabKeyboard(display);
  }

  Keyboard::Keyboard(const Keyboard& keyboard) = default;
  Keyboard& Keyboard::operator=(const Keyboard& keyboard) = default;

  auto Keyboard::input() const -> const std::string& {
    return _text;
  }

  void Keyboard::setWindow(Display* display, Window window) {
    XIM xim = XOpenIM(display, nullptr, nullptr, nullptr);
    if (!xim) {
      throw std::runtime_error("XOpenIM faled: could not open input device");
    }
    _ic = XCreateIC(
      xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
      XNClientWindow, window, XNFocusWindow, window, NULL);
  }

  void Keyboard::processKey(XKeyEvent& keypress) {
    char buffer[32];
    KeySym keysym;
    Status status;
    int len = XmbLookupString(_ic, &keypress, buffer, sizeof buffer, &keysym, &status);

    switch (status) {
      default: /* XLookupNone, XBufferOverflow */
        return;
      case XLookupChars:
        if (!std::iscntrl(*buffer))
          _text.append(buffer);
        break;
      case XLookupKeySym:
      case XLookupBoth:
        break;
    }

    switch (keysym) {
      case XK_Escape:
        std::exit(1);
      default:
        if (!iscntrl(*buffer))
          _text.append(buffer);
        break;
    }
  }

  void Keyboard::_grabKeyboard(Display* display) {
    using namespace std::chrono_literals;

    for (int i=0; i<1000; ++i) {
      auto res = XGrabKeyboard(
        display, DefaultRootWindow(display),
        True, GrabModeAsync, GrabModeAsync, CurrentTime);
      if (res == GrabSuccess) {
        return;
      }
      std::this_thread::sleep_for(1ms);
    }
    throw std::runtime_error("Cannot grab keyboard.");
  }

  Dmenu::Dmenu(Display* display, Config& config) :
    _drawable(_makeDrawable(display, config)),
    _size(), _paddingLR(_drawable.fontHeight()),
    _paddingTB(2), _window(), _schemes{}
  {
    auto it = config.colors.begin();
    while (it != config.colors.end()) {
      _schemes.emplace_back(_drawable, *it++, *it++);
    }

    _size.height = _drawable.fontHeight() + _paddingTB;
    _size.width = config.size.width;

    XSetWindowAttributes swa;
    swa.override_redirect = true;
    swa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    swa.background_pixel = _schemes.front().background()->pixel;

    _window = XCreateWindow(
      display, DefaultRootWindow(display),
      config.pos.x, config.pos.y,
      config.size.width, config.size.height, 0,
      CopyFromParent, CopyFromParent, CopyFromParent,
      CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

    XClassHint ch = { "dmenu", "dmenu" };
    XSetClassHint(display, _window, &ch);

    XMapRaised(display, _window);
    // embed?
    _drawable.resize(config.size);
    draw("");

    // inputwidth
    // promptwidth
    // lines
  }

  auto Dmenu::window() -> Window {
    return _window;
  }

  suckless::drawable& Dmenu::_makeDrawable(Display* display, Config& config) {
    auto screen = DefaultScreen(display);
    auto root   = RootWindow(display, screen);

    XWindowAttributes wa;
    XGetWindowAttributes(display, root, &wa);

    suckless::rect s = {
      static_cast<unsigned int>(wa.width),
      static_cast<unsigned int>(wa.height)
    };
    if (config.size.width == 0)
      config.size.width = wa.width;
    static suckless::drawable drawable{display, screen, root, s};
    drawable.setFontSet(config.fonts);
    return drawable;
  }

  void Dmenu::_clear() {
    _drawable.setColorScheme(_schemes[0]);
    _drawable.draw_rectangle({0, 0}, _size, true, true);
  }

  auto Dmenu::_drawPrompt(unsigned int x) -> unsigned int {
    _drawable.setColorScheme(_schemes[1]);
    return _drawable.draw_text({x, 0}, _size, 5, "Prompt:", false);
  }

  auto Dmenu::_drawInput(const std::string& input, unsigned int x) -> unsigned int {
    _drawable.setColorScheme(_schemes[0]);
    unsigned int cursor_position = 0;
    auto cp = x
      + _drawable.textWidth(input)
      - _drawable.textWidth(input.substr(cursor_position));
    x = _drawable.draw_text(
      {x, 0}, _size, 5, input, false);
    _drawable.draw_rectangle({cp, 2}, {2, _size.height-4}, true, false);
    return x;
  }

  auto Dmenu::_drawItems(const Items& items, unsigned int x) -> unsigned int {
    const auto& list = items.items();
    _drawable.setColorScheme(_schemes[2]);
    x = _drawable.draw_text({x, 0}, _size, 5, list.front(), false);

    // draw other items
    _drawable.setColorScheme(_schemes[0]);
    for (auto it=list.begin()+1; it!=list.end(); ++it) {
      x = _drawable.draw_text({x, 0}, _size, 20, *it, false);
    }
    return x;
  }

  void Dmenu::map() {
    _drawable.map(_window, {0, 0}, _size);
  }

  void Dmenu::draw(const std::string& input) {
    _clear();

    unsigned int x = 0;
    x = _drawPrompt(x);
    x = _drawInput(input, x);
    _drawable.map(_window, {0, 0}, _size);
  }

  void Dmenu::draw(const std::string& input, const Items& items) {
    _clear();

    unsigned int x = 0;
    x = _drawPrompt(x);
    x = _drawInput(input, x);
    x = _drawItems(items, x);
    _drawable.map(_window, {0, 0}, _size);
  }

  void Dmenu::focus() {
    using namespace std::chrono_literals;

    Window focuswin;
    int revertwin;
    for (int i=0; i<100; ++i) {
      XGetInputFocus(_drawable.display(), &focuswin, &revertwin);
      if (focuswin == _window)
        return;
      XSetInputFocus(_drawable.display(), _window, RevertToParent, CurrentTime);
      std::this_thread::sleep_for(1ms);
    }
    std::cerr << "Cannot grab focus.\n";
    std::exit(1);
  }

  void run(Display* display, Dmenu& menu, Items& items, Keyboard& keyboard) {
    XEvent ev;
    keyboard.setWindow(display, menu.window());

    while (!XNextEvent(display, &ev)) {
      if (XFilterEvent(&ev, menu.window()))
        continue;

      switch (ev.type) {
        case DestroyNotify:
          if (ev.xdestroywindow.window != menu.window())
            break;
          // cleanup
          std::exit(1);
        case Expose:
          if (ev.xexpose.count == 0)
            menu.map();
          break;
        case FocusIn:
          if (ev.xfocus.window != menu.window())
            menu.focus();
          break;
        case KeyPress:
          keyboard.processKey(ev.xkey);
        case SelectionNotify:
          /* if (ev.xselection.property == utf8) */
          /*   paste(); */
          break;
        case VisibilityNotify:
          if (ev.xvisibility.state != VisibilityUnobscured)
            XRaiseWindow(display, menu.window());
          break;
      }
      menu.draw(keyboard.input(), items);
    }
  }

  void parse_args(int& argc, char* argv[], Config& cfg) {
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
