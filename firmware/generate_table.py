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
ADC_MAX = 1023

def celsius_from_adc(adc):
  return 213 - 34.3 * ln(adc)

def adc_from_celsius(celsius):
  return exp((213 - celsius) / 34.3)

with open('celsius_from_adc_table.h', 'w') as f:
  print('#include <stdint.h>', file=f)
  print('const uint16_t celsius_from_adc_table[] = {', file=f)
  print(', '.join('%d' % adc_from_celsius(c) for c in range(0, 100)), file=f)
  print('};', file=f)
