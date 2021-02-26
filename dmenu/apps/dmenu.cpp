#include "../src/dmenu.hpp"
#include "../src/draw.hpp"

#include <array>
#include <iostream>

int main(int argc, char* argv[]) {

  dmenu::Config config = {
    .pos = { 0, 0 },
    .size = { 0, 50 },
    .topbar = true,
    .fonts = {
      "Source Code Pro:size=12",
      "JoyPixels:pixelsize=12:antialias=true:autohint=true"
    },
    .prompt = "",
    .colors = {
      // struct pair fg/bg?
      std::string{"#bbbbbb"}, std::string{"#222222"},
      std::string{"#eeeeee"}, std::string{"#005577"},
      std::string{"#000000"}, std::string{"#00ffff"},
    },
    .lines = 0,
    .delimiters = " "
  };
  dmenu::parse_args(argc, argv, config);

  std::cout << "Config:\n"
    << "x: " << config.pos.x << ", y: " << config.pos.y
    << ", w: " << config.size.width << ", h: " << config.size.height
    << ", top: " << std::boolalpha << config.topbar
    << ", prompt: \"" << config.prompt << '\"'
    << ", lines: " << config.lines << '\n';

  auto display = dmenu::openDisplay();

  auto items = dmenu::Items::readStdin();
  auto keyboard = dmenu::Keyboard(display);
  auto menu = dmenu::Dmenu(display, config);

  dmenu::run(display, menu, items, keyboard);

  return 1; // unreachable
}
