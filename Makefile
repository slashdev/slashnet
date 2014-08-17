# Name: Makefile
# Author: Ferdi van der Werf <ferdi@slashdev.nl>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz

# Device, 20 MHz
DEVICE     = atmega644p
CLOCK      = 20000000

# Default fuses
FUSE_LOW  = 0xE7
FUSE_HIGH = 0xDF
FUSE_EXT  = 0xFF

# Executable
EXECUTABLE  = ethshield

# Directories
SRC_DIR     = src
OBJ_DIR     = obj
BUILD_DIR   = build

# Tune the lines below only if you know what you are doing:
CC          = avr-gcc
CC_FLAGS    = -Wall -Os -gdwarf-2 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
COMPILER    = avr-gcc

SOURCES     = $(wildcard src/**/*.c) $(wildcard src/*.c)
OBJECTS     = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DIRECTORIES = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(sort $(dir $(wildcard $(SRC_DIR)/*/))))

COL_INFO    = tput setaf 2
COL_BUILD   = tput setaf 7
COL_RESET   = tput sgr0

ELF         = $(BUILD_DIR)/$(EXECUTABLE).elf
HEX         = $(BUILD_DIR)/$(EXECUTABLE).hex
LSS         = $(BUILD_DIR)/$(EXECUTABLE).lss

# Build executable
all:	dirs $(HEX) avr-size

# Clean environment
clean:
	rm -f $(ELF) $(HEX) $(LSS) $(OBJECTS)

# Use disasm for debugging
disasm:	$(ELF)
	avr-objdump -d $(ELF)

# Avr-dude
fuse:
	avrdude -c avrisp2 -P usb -B 5 -p $(DEVICE) -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXT):m

dude: $(HEX)
	avrdude -c avrisp2 -P usb -B 1 -p $(DEVICE) -U flash:w:$(HEX):i

restart:
	avrdude -c avrisp2 -P usb -p $(DEVICE)

# BootloadHID
load: all
	bootloadHID -l -f $(HEX)

start: all
	bootloadHID -l -s -f $(HEX)

# Build specific
dirs:
	@$(COL_INFO)
	@echo Create directories in 'obj/'
	@$(COL_BUILD)
	mkdir -p $(DIRECTORIES) $(BUILD_DIR)
	@$(COL_RESET)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo
	@$(COL_INFO)
	@echo Building file: $<
	@$(COL_BUILD)
	$(CC) $(CC_FLAGS) -c $< -o $@
	@$(COL_INFO)
	@echo Finished building: $<
	@$(COL_RESET)

$(ELF): $(OBJECTS)
	@echo
	@$(COL_INFO)
	@echo Building file: $(ELF)
	@$(COL_BUILD)
	$(COMPILER) $(CC_FLAGS) -o $(ELF) $(OBJECTS)
	@$(COL_INFO)
	@echo Finished building: $(ELF)
	@$(COL_RESET)

$(HEX): $(ELF)
	@echo
	@$(COL_INFO)
	@echo Clean old hex and lss
	@rm -f $(HEX) $(LSS)
	@$(COL_BUILD)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)
	avr-objdump -f -d -S -t -j .text -w $(ELF) > $(LSS)
	@$(COL_RESET)

avr-size:
	@echo
	@$(COL_INFO)
	@echo AVR-Size:
	@$(COL_BUILD)
	avr-size --totals $(ELF)
	@$(COL_RESET)

.S.o:
	$(CC) $(CC_FLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	@$(COL_INFO)
	@echo Building file: $<
	@$(COL_BUILD)
	$(CC) $(CC_FLAGS) -S $< -o $@
	@$(COL_INFO)
	@echo Finished building: $<
	@$(COL_RESET)
