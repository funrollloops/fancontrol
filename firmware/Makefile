.PHONY: upload compile

INO=firmware.ino
HEX=build/$(INO).hex
FQBN=attiny:avr:ATtinyX5:cpu=attiny85,clock=internal8
#FQBN=arduino:avr:nano:cpu=atmega328old
PORT=/dev/ttyACM0
BUILD_DIR=build
PROGRAMMER=stk500v1

$(HEX): $(INO) celsius_from_adc_table.h Makefile
	mkdir -p $(BUILD_DIR)/sketch
	touch compile_commands.json
	arduino-cli compile --fqbn $(FQBN) --build-property "build.extra_flags=-DSSD1306_NO_SPLASH -std=c++17" --build-path=$(BUILD_DIR)

compile: $(HEX)

upload: $(HEX)
	avrdude -P $(PORT) -p t85 -c stk500v1 -b 19200 -D -U flash:w:$<:i

	arduino-cli upload port $(PORT) --fqbn $(FQBN) --input-dir $(BUILD_DIR) --programmer $(PROGRAMMER)

celcius_from_adc_table.h: generate_table.py
	python3 $<

monitor:
	arduino-cli monitor -p $(PORT)

compile_commands.json: fancontrol.ino
	arduino-cli compile --fqbn $(FQBN) --build-properties build.path=$(PWD)/build $<
