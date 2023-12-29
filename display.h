#pragma once

#define SSD1306_NO_SPLASH 1

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

class Display : public Adafruit_SSD1306 {
  static constexpr uint8_t SCREEN_ADDRESS = 0x3C;

public:
  Display() : Adafruit_SSD1306(128, 64, &Wire, -1) {}
  void init() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
    }
    Serial.println("Display initialized");
    dim(true);

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display();
    delay(2000);  // Pause for 2 seconds

    // Clear the buffer
    clearDisplay();
    display();
  }
};
