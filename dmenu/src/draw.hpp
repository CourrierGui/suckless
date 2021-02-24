#pragma once

#include <cstddef>
#include <list>
#include <initializer_list>
#include <string>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

namespace suckless {

  enum class Color {
    Fg = 0,
    Bg = 1
  };

  struct position {
    unsigned int x, y;
  };

  struct rect {
    unsigned int width, height;
  };


  class drawable;

  class font {
    public:
      font(const drawable& d, const std::string& fname);
      font(const drawable& d, FcPattern* pattern);
      font& operator=(const font& f);
      font(const font& f);

      ~font();

      auto getExtents(const std::string& text, unsigned int len) -> rect;
      auto height() -> unsigned int;
      auto xfont() const -> XftFont*;

    private:
      Display*     _display;
      XftFont*     _xfont;
      FcPattern*   _pattern;
      unsigned int _height;
  };

  class fontset {
    public:
      fontset();
      fontset(drawable& d, const std::vector<std::string>& fonts);

    private:
      std::list<font> _fonts;
  };

  class color_scheme {
    public:
      color_scheme();
      color_scheme(const drawable& d, const std::vector<std::string>& names);
      XftColor* operator[](Color index);

    private:
      XftColor loadColor(const drawable& d, const std::string& name);
      std::vector<XftColor> _colors;
  };

  class cursor {
    public:
      cursor(const drawable& d, int shape);
      cursor(const cursor& c);
      cursor& operator=(const cursor& c);
      ~cursor();

    private:
      Display* _display;
      Cursor   _cursor;
  };

  class drawable {
    public:
      drawable(Display* display, int screen, Window window, rect size);
      drawable(const drawable& d);
      drawable& operator=(const drawable& d);
      ~drawable();
      void resize(rect size);
      void map(Window window, position pos, rect size);
      auto getWidth(const std::string& text) -> unsigned int;

      Display* display() const;
      auto screen() const -> int;

      void setFontSet(const std::vector<std::string>& fonts);
      auto fontHeight() -> unsigned int;
      void setColorScheme(const color_scheme& cs);

      void draw_text(
        const position& p, const rect& size,
        unsigned char left_padding,
        const std::string& text, bool invert);

      void draw_rectangle(
        const position& p, const rect& size,
        bool filled, bool invert);

    private:
      rect         _size;
      Display*     _display;
      int          _screen;
      Window       _root;
      Drawable     _drawable;
      GC           _gc;
      color_scheme _color;
      std::vector<font> _fontset;
  };

} /* end of namespace suckless */
