#include "main.h"

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "stm32f1xx_hal.h"

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  while (1) {
    HAL_Delay(1000);
    for (int i = 0; i < 10; i++) {
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      HAL_Delay(250);
    }
  }
  return 0;
}
