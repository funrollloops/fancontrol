#include "display.h"

constexpr static int PWM_PIN = 5;
constexpr static int TACH_PIN = 2;

static volatile uint16_t tach_count = 0;

static Display display;

void tach_isr() {
  ++tach_count;
}

// Returns the RPM.
void run_fan_at_percent(uint8_t pct) {
  analogWrite(PWM_PIN, uint16_t(pct)*255/100);
}

uint16_t estimate_rpm() {
  tach_count = 0;
  delay(1000);
  // Each revolution is 2 pulses; We sampled for 1s, so multiply by 60.
  return tach_count * uint16_t(60 / 2);
}

void update_display(uint16_t pct, uint16_t rpm, uint16_t temp_c) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(pct);
  display.print("% ");
  display.print(rpm);
  display.print("rpm");
  display.setCursor(0, 24);
  display.setTextSize(4);
  display.print(temp_c);
  display.println("C");
  display.display();
}

uint16_t read_temperature() {
  ADCSRA |= _BV(ADSC); // start the conversion
  while (bit_is_set(ADCSRA, ADSC)); // ADSC is cleared when the conversion finishes
  // Offset 345 => 25
  return (ADCL | (ADCH << 8)) - 320; // combine bytes & correct for temp offset (approximate)}
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PWM_PIN, INPUT);
  // Need pullup
  pinMode(TACH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), tach_isr, RISING);
  Serial.begin(9600);
  ADMUX = 0xC8;
  display.init();
}

void loop() {
  for (int16_t i = 0 ; i <= 100; i += 1) {
    uint16_t rpm = estimate_rpm();
    run_fan_at_percent(i);
    update_display(i, rpm, read_temperature());
  }
}
