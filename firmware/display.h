#pragma once

#define SSD1306_NO_SPLASH 1
#define SSD1306 0x3C

#include <TinyWireM.h>
#include <Tiny4kOLED.h>

class Display {
  static constexpr uint8_t SCREEN_ADDRESS = 0x3C;

public:
  Display() {}

  void init() {
    oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
    oled.clear();
    oled.on();
    oled.switchRenderFrame();
  }
};
