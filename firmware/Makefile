.PHONY: upload

FQBN=arduino:avr:nano:cpu=atmega328old
PORT=/dev/ttyUSB0

upload: firmware.ino celsius_from_adc_table.h
	arduino-cli compile --fqbn $(FQBN) --build-property "build.extra_flags=-DSSD1306_NO_SPLASH -std=c++17" $<
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $<

celsius_from_adc_table.h: generate_table.py
	python3 $<

monitor:
	arduino-cli monitor -p $(PORT)

compile_commands.json: fancontrol.ino
	arduino-cli compile --fqbn $(FQBN) --build-properties build.path=$(PWD)/build $<
