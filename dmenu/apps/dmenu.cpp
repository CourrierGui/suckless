#include "../src/dmenu.hpp"
#include "../src/draw.hpp"

#include <array>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;
#include <iostream>

int main(int argc, char* argv[]) {

  dmenu::config config = {
    .pos = { 0, 0 },
    .size = { 0, 50 },
    .topbar = true,
    .fonts = {
      "Source Code Pro:size=12",
      "JoyPixels:pixelsize=12:antialias=true:autohint=true"
    },
    .prompt = "",
    .colors = {
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
    << ", w: " << config.size.width << ", h: " << config.size.height << '\n';

  auto d = dmenu::menu::create(config);

  std::this_thread::sleep_for(2s);

  return 0; // unreachable
}
