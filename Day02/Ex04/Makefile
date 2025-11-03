SRC = main.c utils.c uart_utils.c led_utils.c
OBJ = $(SRC:.c=.o)
TARGET = main

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

# Rule 'hex' - compiles sources to objects, links to binary, then generates hex
hex: $(TARGET).hex

# Compile each .c file to .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link all object files into one binary
$(TARGET).bin: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Generate hex file from binary
$(TARGET).hex: $(TARGET).bin
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Rule 'flash' - copies main.hex to the microcontroller's flash
flash: $(TARGET).hex
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -b $(PROGRAMMER_BAUD) -U flash:w:$(TARGET).hex

# Rule 'clean' - deletes generated files
clean:
	rm -f $(TARGET).hex $(TARGET).bin $(OBJ)

.PHONY: all hex flash clean