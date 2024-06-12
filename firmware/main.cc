#include "main.h"

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "adc_to_celsius.h"

enum ADC_MODE {
  ADC_MODE_POLLING,
  ADC_MODE_INTERRUPT,
  ADC_MODE_DMA,
};

static constexpr ADC_MODE kAdcMode = ADC_MODE_INTERRUPT;
uint32_t adc_val;
uint16_t tach1 = 0;
uint16_t tach2 = 0;

void set_pwm(uint32_t val) { TIM2->CCR4 = val; }

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();

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
    auto t1 = tach1;
    auto t2 = tach2;

    for (int i = 0; i < 10; i++) {
      // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      HAL_Delay(250);
      if constexpr (kAdcMode == ADC_MODE_POLLING) {
        HAL_ADC_PollForConversion(&hadc2, 1);
        HAL_ADC_ConvCpltCallback(&hadc2);
      }
    }
  }
  return 0;
}

extern "C" {
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc == &hadc2) {
    adc_val = HAL_ADC_GetValue(&hadc2);
    TIM2->CCR4 = (adc_val * 100) >> 12;
  }
}

void ADC_IRQHandler(ADC_HandleTypeDef *hadc) { HAL_ADC_IRQHandler(hadc); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
    case TACH1_Pin:
      tach1++;
      break;
    case TACH2_Pin:
      tach2++;
      break;
  }
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
}
