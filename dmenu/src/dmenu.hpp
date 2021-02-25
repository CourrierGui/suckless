#pragma once

#include "draw.hpp"

#include <vector>

namespace dmenu {

  struct Config {
    suckless::position pos;
    suckless::rect     size;
    bool               topbar;

    std::vector<std::string> fonts;
    std::string prompt;
    std::vector<std::string> colors;
    unsigned int lines;
    std::string delimiters;
  };

  class Items {
    public:
      static Items readStdin();
      void match(const std::string& input);
      auto items() const -> const std::vector<std::string>&;

    private:
      Items(std::vector<std::string>&& items);
      std::vector<std::string> _items;
  };

  class Keyboard {
    public:
      Keyboard(Display* display);
      Keyboard(const Keyboard& keyboard);
      Keyboard& operator=(const Keyboard& keyboard);

      void processKey(XKeyEvent& keypress);
      void setWindow(Display* display, Window window);
      auto input() const -> const std::string&;

    private:
      std::string _text;
      XIC         _ic;

      void _grabKeyboard(Display* display);
  };

  class Dmenu {
    public:
      Dmenu(Display* display, Config& config);
      void draw(const std::string& text, const Items& items);
      void draw(const std::string& text);
      auto window() -> Window;
      void map();
      void focus();

    private:
      static auto _makeDrawable(Display* display, Config& config) -> suckless::drawable&;

      suckless::drawable& _drawable;
      suckless::rect     _size;
      unsigned int       _paddingLR;
      unsigned int       _paddingTB = 2;
      Window             _window;
      std::vector<suckless::color_scheme> _schemes;

      void _clear();
      auto _drawPrompt(unsigned int x) -> unsigned int;
      auto _drawInput(const std::string& text, unsigned int x) -> unsigned int;
      auto _drawItems(const Items& items, unsigned int x) -> unsigned int;
  };

  auto openDisplay() -> Display*;
  void parse_args(int& argc, char* argv[], Config& cfg);
  void run(Display* display, Dmenu& d, Items& i, Keyboard& k);

} /* end of namespace dmenu */
