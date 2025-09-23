# Enhanced Snake Game Makefile for GBA

# Set devkitPro path for Windows if not already set
ifeq ($(DEVKITPRO),)
DEVKITPRO := C:/devkitPro
endif

# Toolchain paths
PREFIX := arm-none-eabi-
CC := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
AS := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)as
LD := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
OBJCOPY := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)objcopy
GBAFIX := $(DEVKITPRO)/tools/bin/gbafix

# Project settings
TARGET := snake
BUILD := build
SOURCES := source
INCLUDES := include

# Find all source files
CFILES := $(wildcard $(SOURCES)/*.c)
SFILES := $(wildcard $(SOURCES)/*.s)

# Object files
OFILES := $(CFILES:$(SOURCES)/%.c=$(BUILD)/%.o) $(SFILES:$(SOURCES)/%.s=$(BUILD)/%.o)

# Compiler flags
CFLAGS := -g -Wall -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer -ffast-math
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -I$(INCLUDES) -I$(DEVKITPRO)/libgba/include

# Linker flags
LDFLAGS := -specs=gba.specs -g -mthumb -mthumb-interwork

# Libraries
LIBS := -L$(DEVKITPRO)/libgba/lib -lgba

# Default target
all: $(TARGET).gba

# Create build directory
$(BUILD):
	@mkdir -p $(BUILD)

# Compile C files
$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	@echo compiling $(notdir $<)
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
$(BUILD)/%.o: $(SOURCES)/%.s | $(BUILD)
	@echo assembling $(notdir $<)
	@$(AS) $(ASFLAGS) -c $< -o $@

# Link
$(BUILD)/$(TARGET).elf: $(OFILES)
	@echo linking $(notdir $@)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

# Convert to GBA
$(TARGET).gba: $(BUILD)/$(TARGET).elf
	@echo built ... $(notdir $@)
	@$(OBJCOPY) -O binary $< $@
	@$(GBAFIX) $@ -t$(GAME_TITLE) -c$(GAME_CODE) -m$(MAKER_CODE)

# Clean
clean:
	@rm -rf $(BUILD)
	@rm -f $(TARGET).gba

.PHONY: all clean