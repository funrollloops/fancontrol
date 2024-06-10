#!/usr/bin/env python3

# calculate prescaler s.t. we get 25khz with 100 arr.

fclk = 48_000_000
arr = 100
prescaler = 20
pwm_freq = 25_000
print(f"Prescaler for {pwm_freq/1_000}kHz:", fclk / (pwm_freq * arr))
print("Actual pwm freq:", fclk / arr / prescaler)

