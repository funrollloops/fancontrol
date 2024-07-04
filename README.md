## Overview
v1 used an arduino nano with a custom PCB on top, but the result was thick and ungainly. v2 uses a pcb with an stm32 integrated on board,
but still expects to use an OLED on a 4-pin carrier board. This does fit inside a Meshlicious alongside a 280mm rad, but it could be more elegant.

v2 includes a USB port, but it's not really used by the firmware. Removing it in v3 might allow for a sleeker design that is easier to put in a case.

### Clearance inside Meshlicious
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


## Schematic

See the pcb/ directory. Designed for PCBA via JLCPCB, using only one extended part (the USB-C connector). Costs $20 shipped for 5x.

## Firmware

Install [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) somewhere, and then build and flash with
```
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release -GNinja -DSTM32_CUBE_PROGRAMMER=/path/to/STM32_Programmer_CLI
ninja -Cbuild flash
```

## Design

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
