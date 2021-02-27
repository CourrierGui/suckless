#include "dmenu.hpp"
#include "draw.hpp"

#include <sstream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <algorithm>

#include <bitset>
#include <iostream>

//TODO display
//- handle multi line display
//
//TODO text input
//- insert at cursor position
//- move cursor
//
//TODO output
//- write selected match on stdin when user hit Enter
//- change selected word with arrow keys
//
//TODO style
//- switch all attributes and methods to caml case

namespace dmenu {

  auto openDisplay() -> Display* {
    auto d = XOpenDisplay(nullptr);
    if (!d) {
      std::cerr << "Could not open display.\n";
      std::exit(1);
    }
    return d;
  }

  Items::Items(std::vector<Item>&& items) :
    _items{std::move(items)} {  }

  Items Items::readStdin() {
    std::string buffer;
    std::vector<Items::Item> items;

    int i = 0;
    while (std::getline(std::cin, buffer)) {
      items.push_back({buffer, i++, Items::Tag::Out});
      buffer.clear();
    }
    return Items(std::move(items));
  }

  auto Items::items() const -> const std::vector<Item>& {
    return _items;
  }

  bool contains_first_token(const std::string& s, const Items::Item& i) {
    auto token = s.substr(0, s.find_first_of(' '));
    return token == i.text.substr(0, token.size());
  }

  bool contains_all_tokens(const std::string& s, const Items::Item& i) {
    std::stringstream ss(s);
    std::string token;
    token.reserve(s.size());
    while (std::getline(ss, token, ' ')) {
      if (i.text.find(token) == std::string::npos)
        return false;
    }
    return true;
  }

  //FIXME match has O(n log(n)) complexity but it could be O(n) I think
  // But everything is done in place so it may be faster than the other method
  // must be tested
  //TODO should the order of the matches be taken into account?
  //-> not done in the original dmenu
  void Items::match(const std::string& input) {
    auto key = [&input, n=_items.size()](Item& i) -> int {
      if (input == i.text) {
        i.tag = Items::Tag::Match;
        return i.position; // exact match first

      } else if (contains_first_token(input, i)) {
        i.tag = Items::Tag::Prefix;
        return i.position +   n; // then prefix

      } else if (contains_all_tokens(input, i)) {
        i.tag = Items::Tag::Substr;
        return i.position + 2*n; // then substrings

      } else {
        i.tag = Items::Tag::Out;
        return i.position + 3*n; // not inside
      }
    };

    auto compare = [&key](Item& lhs, Item& rhs) -> bool {
      return key(lhs) < key(rhs);
    };
    std::sort(_items.begin(), _items.end(), compare);
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
      throw std::runtime_error("XOpenIM failed: could not open input device");
    }
    _ic = XCreateIC(
      xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
      XNClientWindow, window, XNFocusWindow, window, NULL);
  }

  void display(char c) {
    std::cout
      << c << ' '
      << std::bitset<8>(c) << ' '
      << std::bitset<8>(c & 0b11000000) << ' '
      << ((c & 0b11000000) == 0b10000000) << '\n';
  }

  auto lastUtf8CharSize(const std::string& str) -> size_t {
    size_t size = 1;
    auto it = str.end();;
    --it;
    while ((*it & 0b11000000) == 0b10000000) {
      --it;
      ++size;
    }
    return size;
  }

  void Keyboard::processKey(XKeyEvent& keypress, Items& items, unsigned int& cp) {
    char buffer[32];
    KeySym keysym;
    Status status;
    int len = Xutf8LookupString(
      _ic, &keypress, buffer, sizeof buffer, &keysym, &status);

    switch (status) {
      default: /* XLookupNone, XBufferOverflow */
        return;
      case XLookupChars:
        if (!std::iscntrl(*buffer)) {
          _text.append(buffer, len);
          items.match(_text);
          cp += len;
        }
        break;
      case XLookupKeySym:
      case XLookupBoth:
        break;
    }

    switch (keysym) {
      case XK_Escape:
        std::exit(1);
      case XK_BackSpace:
        if (cp > 0) {
          auto size = lastUtf8CharSize(_text);
          cp -= size;
          _text.resize(_text.size() - size);
          items.match(_text);
        }
        break;
      default:
        if (!iscntrl(*buffer)) {
          for (int i=0; i<len; ++i) {
            display(buffer[i]);
          }
          _text.append(buffer, len);
          cp += len;
          items.match(_text);
        }
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
    _paddingTB(2), _inputWidth(config.size.width/3),
    _window(), _prompt{config.prompt}, _schemes()
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
    draw("", 0);

    // inputwidth
    // promptwidth
    // lines
  }

  auto Dmenu::window() -> Window {
    return _window;
  }

  sl::drawable& Dmenu::_makeDrawable(Display* display, Config& config) {
    auto screen = DefaultScreen(display);
    auto root   = RootWindow(display, screen);

    XWindowAttributes wa;
    XGetWindowAttributes(display, root, &wa);

    sl::rect s = {
      static_cast<unsigned int>(wa.width),
      static_cast<unsigned int>(wa.height)
    };
    if (config.size.width == 0)
      config.size.width = wa.width;
    static sl::drawable drawable{display, screen, root, s};
    drawable.setFontSet(config.fonts);
    return drawable;
  }

  void Dmenu::_clear() {
    _drawable.setColorScheme(_schemes[0]);
    _drawable.draw_rectangle({0, 0}, _size, true, true);
  }

  auto Dmenu::_drawPrompt(unsigned int x) -> unsigned int {
    if (_prompt.empty())
      return x;
    _drawable.setColorScheme(_schemes[1]);
    auto width = _drawable.getWidth(_prompt) + 3 * _paddingLR / 4;
    return _drawable.draw_text(
      {x, 0}, {width, _size.height}, _paddingLR / 2, _prompt, false);
  }

  auto Dmenu::_drawInput(
    const std::string& input,
    unsigned int x,
    unsigned int cp)
    -> unsigned int
  {
    auto cursor =
      _paddingLR/2-1 +
      _drawable.getWidth(input) - _drawable.getWidth(input.substr(cp));

    _drawable.setColorScheme(_schemes[0]);
    _drawable.draw_text(
      {x, 0}, {_inputWidth, _size.height}, _paddingLR / 2, input, false);
    _drawable.draw_rectangle({x+cursor, 2}, {2, _size.height-4}, true, false);
    return x+_inputWidth;
  }

  auto Dmenu::_drawItems(const Items& items, unsigned int x) -> unsigned int {
    const auto& list = items.items();
    _drawable.setColorScheme(_schemes[2]);
    auto s = sl::rect{
      _drawable.getWidth(list.front().text)+_paddingLR,
        _size.height
    };
    x = _drawable.draw_text({x, 0}, s, _paddingLR / 2, list.front().text, false);
    // draw Out tags only if they are all outs
    // (i.e. the first one is out, because outs are last)
    bool draw_outs = list.front().tag == Items::Tag::Out;

    // draw other items
    _drawable.setColorScheme(_schemes[0]);
    for (auto it=list.begin()+1; it!=list.end(); ++it) {
      if (!draw_outs && it->tag == Items::Tag::Out)
        break;
      auto w = _drawable.getWidth(it->text)+_paddingLR;
      if (x + w > _size.width)
        break;
      x = _drawable.draw_text(
        {x, 0}, {w, _size.height}, _paddingLR / 2, it->text, false);
    }
    return x;
  }

  void Dmenu::map() {
    _drawable.map(_window, {0, 0}, _size);
  }

  void Dmenu::draw(const std::string& input, unsigned int cp) {
    _clear();

    unsigned int x = 0;
    x = _drawPrompt(x);
    x = _drawInput(input, x, cp);
    _drawable.map(_window, {0, 0}, _size);
  }

  void Dmenu::draw(const std::string& input, unsigned int cp, const Items& items) {
    _clear();

    unsigned int x = 0;
    x = _drawPrompt(x);
    x = _drawInput(input, x, cp);
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
    unsigned int cp = 0;

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
            menu.draw(keyboard.input(), cp, items);
          break;
        case FocusIn:
          if (ev.xfocus.window != menu.window())
            menu.focus();
          break;
        case KeyPress:
          keyboard.processKey(ev.xkey, items, cp);
          menu.draw(keyboard.input(), cp, items);
        case SelectionNotify:
          /* if (ev.xselection.property == utf8) */
          /*   paste(); */
          break;
        case VisibilityNotify:
          if (ev.xvisibility.state != VisibilityUnobscured)
            XRaiseWindow(display, menu.window());
          break;
      }
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
      } else if (!strcmp(arg, "-p")) {
        cfg.prompt = argv[++i];
      } else if (!strcmp(arg, "-l")) {
        cfg.lines = atoi(argv[++i]);
      } else if (!strcmp(arg, "-b")) {
        cfg.topbar = false;
      } else {
      }
    }
  }

} /* end of namespace dmenu */
