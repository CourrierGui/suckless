#include "draw.hpp"
#include <iostream>

namespace suckless {

  drawable::drawable(Display* display, int screen, Window root, rect size) :
    _size{size}, _display{display}, _screen{screen}, _root{root},
    _drawable{}, _gc{}, _color{}, _fontset{}
  {
    _drawable = XCreatePixmap(
      _display, _root,
      _size.width, _size.height,
      DefaultDepth(_display, _screen));
    _gc = XCreateGC(_display, _root, 0, nullptr);

    XSetLineAttributes(_display, _gc, 1, LineSolid, CapButt, JoinMiter);
  }

  drawable& drawable::operator=(const drawable& d) {
    _size = d._size;
    _display = d._display;
    _screen = d._screen;
    _root = d._root;
    _drawable = d._drawable;
    _gc = d._gc;
    _color = d._color;
    _fontset = d._fontset;
    return *this;
  }

  drawable::drawable(const drawable& d) :
    _size{d._size}, _display{d._display}, _screen{d._screen}, _root{d._root},
    _drawable{d._drawable}, _gc{d._gc}, _color{d._color}, _fontset{d._fontset} {  }

  drawable::~drawable() {
    XFreePixmap(_display, _drawable);
    XFreeGC(_display, _gc);
  }

  void drawable::resize(rect size) {
    _size = size;

    if (_drawable)
      XFreePixmap(_display, _drawable);

    _drawable = XCreatePixmap(
      _display, _root,
      _size.width, _size.height,
      DefaultDepth(_display, _screen));
  }

  void drawable::map(Window window, position pos, rect size) {
    XCopyArea(
      _display, _drawable, window, _gc,
      pos.x, pos.y, size.width, size.height,
      pos.x, pos.y);
    XSync(_display, false);
  }

  auto drawable::fontHeight() -> unsigned int {
    return _fontset.front().height();
  }

  Display* drawable::display() const {
    return _display;
  }

  auto drawable::screen() const -> int {
    return _screen;
  }

  auto drawable::getWidth(const std::string& text) -> unsigned int {
    return 0;
  }

  void drawable::setFontSet(const std::vector<std::string>& fonts) {
    for (const auto& f: fonts) {
      _fontset.push_back(font(*this, f));
    }
  }

  void drawable::setColorScheme(const color_scheme& cs) {
    _color = cs;
  }

  void drawable::draw_text(
    const position& p, const rect& size,
    unsigned char left_padding,
    const std::string& text, bool invert)
  {
    XftDraw* d = XftDrawCreate(
      _display, _drawable,
      DefaultVisual(_display, _screen),
      DefaultColormap(_display, _screen));

    XftDrawStringUtf8(
      d, _color[Color::Bg],
      _fontset.front().xfont(), p.x, p.y,
      (XftChar8*)text.c_str(), text.size());
  }

  void drawable::draw_rectangle(
    const position& p, const rect& size,
    bool filled, bool invert)
  {
    XSetForeground(
      _display, _gc, invert ? _color[Color::Bg]->pixel : _color[Color::Fg]->pixel);

    if (filled)
      XFillRectangle(_display, _drawable, _gc, p.x, p.y, size.width, size.height);
    else
      XDrawRectangle(_display, _drawable, _gc, p.x, p.y, size.width-1, size.height-1);
  }

  font::font(const drawable& d, const std::string& fname) :
    _display{d.display()}, _xfont{nullptr}, _pattern{nullptr}, _height{0}
  {
    _xfont   = XftFontOpenName(_display, d.screen(), fname.c_str());
    _pattern = FcNameParse((FcChar8*)fname.c_str());
    if (!_xfont) {
      std::cerr << "Error: cannot load font from name: " << fname << '\n';
      return; // throw?
    }
    if (!_pattern) {
      std::cerr << "Error: cannot parser font name to pattern: " << fname << '\n';
      XftFontClose(_display, _xfont);
      return; // throw?
    }
    _height = _xfont->ascent + _xfont->descent;
  }

  font::font(const drawable& d, FcPattern* pattern) :
    _display{d.display()}, _xfont{nullptr}, _pattern{nullptr}, _height{0}
  {
    _xfont = XftFontOpenPattern(_display, pattern);
    if (!_xfont) {
      std::cerr << "Error: cannot parse font from pattern.\n";
      return; // throw?
    }
    _height = _xfont->ascent + _xfont->descent;
  }

  font& font::operator=(const font& f) {
    _display = f._display;
    _xfont = f._xfont;
    _pattern = f._pattern;
    _height = f._height;
    return *this;
  }

  font::font(const font& f) :
    _display{f._display}, _xfont{f._xfont}, _pattern{f._pattern}, _height{f._height}
  {

  }

  font::~font() {
    if (_pattern)
      FcPatternDestroy(_pattern);
    XftFontClose(_display, _xfont);
  }

  auto font::getExtents(const std::string& text, unsigned int len)
    -> rect
  {
    XGlyphInfo ext;
    XftTextExtentsUtf8(
      _display, _xfont,
      (XftChar8*)text.c_str(),
      len, &ext);
    return {static_cast<unsigned int>(ext.xOff), _height};
  }

  auto font::xfont() const -> XftFont* {
    return _xfont;
  }

  auto font::height() -> unsigned int {
    return _height;
  }

  fontset::fontset() : _fonts{} {  }

  fontset::fontset(drawable& d, const std::vector<std::string>& fonts) :
    _fonts{}
  {
    for (const auto& fname: fonts) {
      _fonts.push_front(font{d, fname});
    }
  }

  color_scheme::color_scheme() : _colors{} {  }

  color_scheme::color_scheme(
    const drawable& d,
    const std::vector<std::string>& names) :
    _colors{}
  {
    // we need at least two colors to make a color scheme
    if (names.size() < 2)
      return; // throw?

    for (const auto& c: names)
      _colors.push_back(loadColor(d, c));
  }

  XftColor* color_scheme::operator[](Color index) {
    return &_colors[static_cast<std::size_t>(index)];
  }

  XftColor color_scheme::loadColor(const drawable& d, const std::string& name) {
    XftColor xcolor;
    auto res = XftColorAllocName(
      d.display(), DefaultVisual(d.display(), d.screen()),
      DefaultColormap(d.display(), d.screen()),
      name.c_str(), &xcolor);

    if (!res) {
      std::cerr << "Error: cannot allocate color " << name << '\n';
      // throw?
    }
    return xcolor;
  }

  cursor::cursor(const drawable& d, int shape) :
    _display{d.display()},
    _cursor{XCreateFontCursor(_display, shape)}
  {
  }

  cursor::cursor(const cursor& c) :
    _display{c._display},
    _cursor{c._cursor} {  }

  cursor& cursor::operator=(const cursor& c) {
    _display = c._display;
    _cursor = c._cursor;
    return *this;
  }

  cursor::~cursor() {
    XFreeCursor(_display, _cursor);
  }

} /* end of namespace suckless */
