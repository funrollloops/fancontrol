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
  MX_TIM1_Init();
  while (1) {
  }
  return 0;
}
