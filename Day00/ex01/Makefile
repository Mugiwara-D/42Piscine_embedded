# Variables
MCU = atmega328p
F_CPU = 16000000UL
PORT = /dev/ttyUSB0
PROGRAMMER_BAUD = 115200

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

# Rule 'all' - executes hex then flash
all: hex flash

# Rule 'hex' - compiles main.c to main.bin, then generates main.hex from main.bin
hex: main.hex

main.bin: main.c
	$(CC) $(CFLAGS) -o main.bin main.c

main.hex: main.bin
	$(OBJCOPY) -O ihex -R .eeprom main.bin main.hex

# Rule 'flash' - copies main.hex to the microcontroller's flash
flash: main.hex
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -b $(PROGRAMMER_BAUD) -U flash:w:main.hex

# Rule 'clean' - deletes main.hex and main.bin
clean:
	rm -f main.hex main.bin

.PHONY: all hex flash clean