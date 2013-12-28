# Name: Makefile
# Author: Ferdi van der Werf <ferdi@slashdev.nl>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".

DEVICE     = atmega644p
CLOCK      = 20000000 # 20 Mhz

OUTPUT_DIR   = Debug/
OUTPUT_HEX   = EthShield.hex
OUTPUT_ELF   = EthShield.elf
OUTPUT_LSS   = EthShield.lss
FUSE_LOW  = 0xE7
FUSE_HIGH = 0xDF
FUSE_EXT  = 0xFF

OBJECTS    = \
com/uart.o \
utils/debug.o \
EthShield.o 

# Tune the lines below only if you know what you are doing:

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	$(OUTPUT_HEX)


.c.o:
	@tput setaf 1
	@echo Building file: $<
	@tput setaf 7
	$(COMPILE) -c $< -o $@
	@tput setaf 1
	@echo Finished building: $<
	@tput sgr0

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	@tput setaf 1
	@echo Building file: $<
	@tput setaf 7
	$(COMPILE) -S $< -o $@
	@tput setaf 1
	@echo Finished building: $<
	@tput sgr0

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID -l -f $(OUTPUT_DIR)$(OUTPUT_HEX)

start: all
	bootloadHID -l -s -f $(OUTPUT_DIR)$(OUTPUT_HEX)

clean:
	rm -f $(OUTPUT_DIR)$(OUTPUT_HEX) $(OUTPUT_DIR)$(OUTPUT_ELF) $(OBJECTS)

fuse:
	avrdude -c avrisp2 -P usb -p $(DEVICE) -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXT):m

dude: $(OUTPUT_HEX)
	avrdude -c avrisp2 -P usb -p $(DEVICE) -U flash:w:$(OUTPUT_DIR)$(OUTPUT_HEX):i

# file targets:
$(OUTPUT_ELF): $(OBJECTS)
	@tput setaf 1
	@echo Building file: $(OUTPUT_ELF)
	@tput setaf 7
	$(COMPILE) -o $(OUTPUT_ELF) $(OBJECTS)
	@tput setaf 1
	@echo Finished building: $(OUTPUT_ELF)
	@tput sgr0

$(OUTPUT_HEX): $(OUTPUT_ELF)
	@rm -f $(OUTPUT_HEX)
	@avr-objcopy -j .text -j .data -O ihex $(OUTPUT_ELF) $(OUTPUT_HEX)
	@avr-objcopy -h -S $(OUTPUT_ELF) > $(OUTPUT_LSS)
	@tput setaf 1
	@echo AVR-Size:
	@tput setaf 7
	avr-size --totals $(OUTPUT_ELF)
	@tput sgr0
	@rm -f $(OBJECTS)
	@mv $(OUTPUT_ELF) $(OUTPUT_DIR).
	@mv $(OUTPUT_HEX) $(OUTPUT_DIR).
	@mv $(OUTPUT_LSS) $(OUTPUT_DIR).

# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	$(OUTPUT_ELF)
	avr-objdump -d $(OUTPUT_DIR)$(OUTPUT_ELF)
