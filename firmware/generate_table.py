#!/usr/bin/env python3
from math import exp, log as ln
# Observed measurements
measurements = [
  [33, 7.28],
  [34, 7],
  [38, 5.9],
  [39, 6.2],
  [40, 5.6],
  [41, 5.4],
  [46, 4.6],
  [49, 4.25],
  [52, 3.8],
  [55, 3.31]
]

R2 = 33e3
ADC_MAX = 1 << 12
MAX_C = 100

def celsius_from_adc(adc):
  return 213 - 34.3 * ln(adc)

def adc_from_celsius(celsius):
  return exp((213 - celsius) / 34.3) * (ADC_MAX / 1024)

with open('adc_to_celsius.c', 'w') as f:
  print('#include "adc_to_celsius.h"', file=f)
  print('const uint16_t adc_for_celsius[] = {', file=f)
  print(', '.join('%d' % adc_from_celsius(c) for c in range(0, MAX_C)), file=f)
  print('};', file=f)
with open('adc_to_celsius.h', 'w') as f:
  print('#include <stdint.h>', file=f)
  print(f'extern const uint16_t adc_for_celsius[{MAX_C}];', file=f)
