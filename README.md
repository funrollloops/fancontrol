## Schematic

WIP

## Firmware

In the firmware directory. Requires arduino-cli and 'Adafruit SSD1306' library to be installed.

[Download the arduino-cli binary](https://github.com/arduino/arduino-cli/releases) and drop it in your path.


Install the library using arduino-cli:

```bash
arduino-cli lib install "Adafruit SSD1306"
```


Build, upload, and monitor with make:
```bash
make upload monitor
```



## Calibration

WIP, will log CPU/GPU/thermistor readings to create a calibration table.  According to the datasheet,
given some inputs the output formula should be:

```py
# Circuit: VCC - R2 - thermistor - GND
#                   ⌞ ADC

VCC = 5
AREF = 3.3
R2 = 33_000

def C(adc):
    """adc is the voltage across the thermistor when it is in series with R2, where 0=0V and 1024=AREF."""
    return 227 - 34.3 * ln(adc)
```

But a lookup table is probably simpler since only ~1C accuracy is needed and there's no FPU anyway.
