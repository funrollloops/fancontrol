#include "display.h"
#include "celsius_from_adc_table.h"

constexpr static int PWM_PIN = 5;
constexpr static int TACH_PIN = 2;
constexpr static int THERMISTOR_PIN = A0;

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

void update_display(uint16_t pct, uint16_t rpm, float temp_c) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(pct);
  display.print("% ");
  if (rpm > 10'000) {
    display.print(rpm / 1024);
    display.print("k");
  } else {
    display.print(rpm);
  }
  display.print("rpm");
  display.setCursor(0, 24);
  display.setTextSize(4);
  display.print(temp_c > 99 ? 99 : temp_c < 0 ? 0 : round(temp_c));
  display.println("C");
  display.display();
}

float read_internal_temperature() {
  ADCSRA |= _BV(ADSC); // start the conversion
  while (bit_is_set(ADCSRA, ADSC)); // ADSC is cleared when the conversion finishes
  return (ADCW - 324.31) / 1.22; // combine bytes & correct for temp offset (approximate)}
}

uint16_t read_thermistor_adc() {
  //return 202 - 32.4 * log(analogRead(A0));
  return analogRead(A0);
}

float thermistor_adc_to_celsius(uint16_t adc) {
  const auto& table = celsius_from_adc_table;
  static constexpr uint8_t MAX = sizeof(table) / sizeof(table[0]);
  static_assert(MAX == 100);
  // Binary search celsius_from_adc_table for the closest value.
  uint8_t low = 0;
  uint8_t high = MAX;
  // The index is the temperature in C, the value is the ADC reading. ADC readings
  // are decreasing with temperature.
  while (low < high) {
    uint8_t mid = (low + high) / 2;
    if (table[mid] == adc) {
      return mid;
    } else if (table[mid] < adc) {
      high = mid;
    } else {
      low = mid + 1;
    }
  }
  if (low <= 0 || low >= MAX) {
    return low;
  }
  return low - (100 * (adc - table[low]) / (table[low - 1] - table[low]) / 100.);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PWM_PIN, INPUT);
  // Need pullup
  pinMode(TACH_PIN, INPUT_PULLUP);
  pinMode(THERMISTOR_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(TACH_PIN), tach_isr, RISING);
  Serial.begin(9600);
  ADMUX = 0xC8;
  display.init();
}

void loop() {
  static uint8_t MIN_SPEED = ceil(.30 * 255);
  auto thermistor_adc = read_thermistor_adc();
  auto thermistor_temp = thermistor_adc_to_celsius(thermistor_adc);
  uint8_t speed = 255;
  if (thermistor_temp > 55) {
    speed = 255;
  } else if (thermistor_temp > 45) {
    // linear from 30% to 100% (255) in this range.
    speed = (255 - MIN_SPEED) * (thermistor_temp - 45) / 10 + MIN_SPEED;
  } else if (thermistor_temp > 20) {
    speed = MIN_SPEED;
  } else {
    speed = 255;  // In case of a bad reading, run at full speed.
  }
  Serial.print("Thermistor adc: ");
  Serial.print(thermistor_adc);
  Serial.print(" temperature: ");
  Serial.println(thermistor_temp);
  auto rpm = estimate_rpm();
  analogWrite(PWM_PIN, speed);
  update_display(static_cast<uint16_t>(speed)*100/255, rpm, thermistor_temp);
}
