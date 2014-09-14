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
CC_FLAGS    = -Wall -Werror -Os -gdwarf-2 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
COMPILER    = avr-gcc

SOURCES     = $(wildcard src/**/*.c) $(wildcard src/*.c)
OBJECTS     = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DIRECTORIES = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(sort $(dir $(wildcard $(SRC_DIR)/*/))))

COL_INFO    = tput setaf 2
COL_BUILD   = tput setaf 7
COL_ERROR   = tput setaf 1
COL_RESET   = tput sgr0

ELF         = $(BUILD_DIR)/$(EXECUTABLE).elf
HEX         = $(BUILD_DIR)/$(EXECUTABLE).hex
LSS         = $(BUILD_DIR)/$(EXECUTABLE).lss

log_info = $(COL_RESET); printf "$(1) "
log_ok   = $(COL_RESET); printf "["; $(COL_INFO);  printf "OK";    $(COL_RESET); printf "]\n"

# Build executable
all:	$(HEX) avr-size

# Help, explains usage
help:
	@echo "Usage:"
	@echo "- all:     Build executable"
	@echo "- help:    Display this help"
	@echo "Using AVR-dude:"
	@echo "- fuse:    Set defined fuses on chip"
	@echo "- dude:    Upload hex to chip"
	@echo "- restart: Restart the chip"
	@echo "Using bootloadHID:"
	@echo "- load:    Upload hex to chip"
	@echo "- start:   (Re)start the chip"

# Clean environment
clean:
	@$(call log_info,"Cleaning...")
	@rm -f $(ELF) $(HEX) $(LSS) $(OBJECTS)
	@$(call log_ok)

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
	@$(call log_info,Create directories in 'obj/')
	@mkdir -p $(DIRECTORIES) $(BUILD_DIR)
	@$(call log_ok)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(call log_info,Compiling $<)
	@$(COL_ERROR)
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@$(call log_ok)

$(ELF): dirs $(OBJECTS)
	@echo
	@$(call log_info,Linking $(ELF))
	@$(COL_ERROR)
	@$(COMPILER) $(CC_FLAGS) -o $(ELF) $(OBJECTS)
	@$(call log_ok)

$(HEX): $(ELF)
	@$(call log_info,Create new hex and lss files)
	@$(COL_ERROR)
	@rm -f $(HEX) $(LSS)
	@avr-objcopy --only-section .text --only-section .data --output-target ihex $(ELF) $(HEX)
	@avr-objdump --file-headers --disassemble --source --syms --section=.text --wide $(ELF) > $(LSS)
	@$(call log_ok)

avr-size:
	@echo
	@$(COL_INFO)
	@echo AVR-Size:
	@$(COL_BUILD)
	avr-size --totals $(ELF)
	@$(COL_RESET)

.S.o:
	@$(CC) $(CC_FLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	@$(call log_info,Compiling $<)
	@$(COL_ERROR)
	@$(CC) $(CC_FLAGS) -S $< -o $@
	@$(call log_ok)
