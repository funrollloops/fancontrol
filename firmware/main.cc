#include "main.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <cstdio>
#include <functional>

#include "adc.h"
#include "adc_to_celsius.h"
#include "gpio.h"
#include "i2c.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

enum ADC_MODE {
  ADC_MODE_POLLING,
  ADC_MODE_INTERRUPT,
  ADC_MODE_DMA,
};

static constexpr ADC_MODE kAdcMode = ADC_MODE_POLLING;
static constexpr bool PRIME = false;
uint32_t adc_val;
std::atomic<uint16_t> tach1 = 0;
std::atomic<uint16_t> tach2 = 0;

void set_pwm_pct(uint32_t val) { TIM2->CCR4 = val; }

int thermistor_adc_to_celsius(uint16_t adc) {
  const auto &table = adc_for_celsius;
  return std::distance(std::begin(table),
                       std::lower_bound(std::begin(table), std::end(table), adc,
                                        std::greater{}));
}

void ssd1306_WriteInt(uint16_t num, SSD1306_Font_t font, SSD1306_COLOR color) {
  char str[10];
  auto r = std::to_chars(std::begin(str), std::end(str), num);
  *r.ptr = '\0';
  ssd1306_WriteString(str, font, color);
}

int write_line(int ypos, SSD1306_Font_t font, const char *prefix, uint16_t num,
               const char *suffix) {
  ssd1306_SetCursor(0, ypos);
  ssd1306_WriteString(prefix, font, White);
  ssd1306_WriteInt(num, font, White);
  ssd1306_WriteString(suffix, font, White);
  return ypos + font.height + 2;
}

uint8_t fan_speed(int temp) {
  static constexpr uint8_t MIN_SPEED = 20;
  static constexpr uint8_t MAX_SPEED = 100;
  static constexpr uint8_t MAX_LINEAR_TEMP = 55;
  static constexpr uint8_t MIN_LINEAR_TEMP = 40;
  if constexpr (!PRIME) {
    if (temp > MAX_LINEAR_TEMP) {
      return MAX_SPEED;
    } else if (temp > MIN_LINEAR_TEMP) {  // Linear range.
      return (MAX_SPEED - MIN_SPEED) * (temp - MIN_LINEAR_TEMP) /
                 (MAX_LINEAR_TEMP - MIN_LINEAR_TEMP) +
             MIN_SPEED;
    } else if (temp > 20) {  // 20C = 68F
      return MIN_SPEED;
    } else {
      return MAX_SPEED;  // In case of a bad reading, run at full speed.
    }
  } else {
    static uint8_t ticks = 0;
    static constexpr uint8_t PERIOD = 64;
    uint8_t qq = ticks++ % PERIOD;
    if (qq < PERIOD / 4) {
      return 255;
    } else if (qq > 3 * PERIOD / 4) {
      return 30;
    } else {
      return 255 - (qq - PERIOD / 4) * (255 - 30) / (PERIOD / 2);
    }
  }
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  ssd1306_Init();

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  if constexpr (kAdcMode == ADC_MODE_POLLING) {
    HAL_ADC_Start(&hadc2);
  } else if constexpr (kAdcMode == ADC_MODE_INTERRUPT) {
    HAL_ADC_Start_IT(&hadc2);
  } else if constexpr (kAdcMode == ADC_MODE_DMA) {
    HAL_ADC_Start_DMA(&hadc2, &adc_val, 2);
  }
  while (1) {
    tach1 = 0;
    tach2 = 0;
    HAL_Delay(1000);
    uint16_t t1 = tach1;
    uint16_t t2 = tach2;
    // Convert to RPM, two pulses per revolution.
    t1 = t1 * 60 / 2;
    t2 = t2 * 60 / 2;
    const int temp = thermistor_adc_to_celsius(adc_val);
    set_pwm_pct(fan_speed(temp));

    if constexpr (kAdcMode == ADC_MODE_POLLING) {
      HAL_ADC_PollForConversion(&hadc2, 1);
      HAL_ADC_ConvCpltCallback(&hadc2);
    }
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 4);
    if (temp < 99) ssd1306_WriteChar(' ', Font_16x26, White);
    if (temp < 9) ssd1306_WriteChar(' ', Font_16x26, White);
    ssd1306_WriteInt(temp, Font_16x26, White);
    ssd1306_WriteString("C ", Font_11x18, White);
    ssd1306_WriteInt(std::min<uint16_t>(99, TIM2->CCR4), Font_16x26, White);
    ssd1306_WriteString("%", Font_11x18, White);

    const int rpm_ypos = 62 - 2 * Font_7x10.height;
    int ypos = rpm_ypos;
    ypos = write_line(ypos, Font_7x10, "", t1, "rpm");
    ssd1306_SetCursor(64, rpm_ypos);
    ssd1306_WriteInt(t2, Font_7x10, White);
    ssd1306_WriteString("rpm", Font_7x10, White);
    float ar = adc_val / 4095.0;
    const float r2 = 33.3e3;
    const float r = ar * r2 / (1 - ar);
    ypos = write_line(ypos, Font_7x10, "ADC=", adc_val, "  R=");
    ssd1306_WriteInt(r, Font_7x10, White);
    ssd1306_UpdateScreen();
  }
  return 0;
}

extern "C" {
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *) {
  adc_val = hadc2.Instance->DR;
}

void ADC_IRQHandler(ADC_HandleTypeDef *hadc) { HAL_ADC_IRQHandler(hadc); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
    case TACH1_Pin:
      tach1.fetch_add(1);
      break;
    case TACH2_Pin:
      tach2.fetch_add(1);
      break;
  }
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
}
