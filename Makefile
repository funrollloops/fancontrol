.PHONY: upload

FQBN=arduino:avr:nano:cpu=atmega328old
PORT=/dev/ttyUSB0

upload: fancontrol.ino
	arduino-cli compile --fqbn $(FQBN) --build-property "build.extra_flags=-DSSD1306_NO_SPLASH" $<
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $<

monitor:
	arduino-cli monitor -p $(PORT)

compile_commands.json: fancontrol.ino
	arduino-cli compile --fqbn $(FQBN) --build-properties build.path=$(PWD)/build $<
