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


## V2: Low Profile version (wip)

v1 used an arduino nano with a custom PCB on top, but the result doesn't fit
well into the case. v2 will use a pcb with an stm32 integrated on board.

Total available clearance is 10mm:

* 1.6mm PCB
* 6.5mm for right angle fan header on back
* 6.3mm for the oled display module
    * 1.6mm for the PCB
    * 1.6mm for the display
    * 3.1mm for the JST connector (1.1mm for components w/o connector)

So will need to use a bare oled display and use an FPC connector to save space.
All components will be on the top side of the PCB, which will face towards the
radiator/fan where the fan connectors come out.
