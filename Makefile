# Multi-Platform Snake Game Makefile
# Supports: GBA, NDS, GameCube (later)

# Default platform
PLATFORM ?= gba

# Set devkitPro path for Windows if not already set
ifeq ($(DEVKITPRO),)
DEVKITPRO := C:/devkitPro
endif

# Project settings
TARGET := snake
BUILD := build
CORE_SRC := core/game.c
PLATFORM_DIR := platform

# Platform-specific configurations
ifeq ($(PLATFORM),gba)
    # GBA Configuration
    PREFIX := arm-none-eabi-
    CC := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
    AS := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)as
    LD := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
    OBJCOPY := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)objcopy
    GBAFIX := $(DEVKITPRO)/tools/bin/gbafix
    
    # Compiler flags
    CFLAGS := -g -Wall -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer -ffast-math
    CFLAGS += -mthumb -mthumb-interwork
    CFLAGS += -I$(PLATFORM_DIR) -Icore -I$(DEVKITPRO)/libgba/include
    
    # Linker flags
    LDFLAGS := -specs=gba.specs -g -mthumb -mthumb-interwork
    LIBS := -L$(DEVKITPRO)/libgba/lib -lgba
    
# Source files
PLATFORM_SRC := $(PLATFORM_DIR)/gba.c
OUTPUT := $(TARGET).gba
BUILD_DIR := $(BUILD)/gba

else ifeq ($(PLATFORM),nds)
    # NDS Configuration
    PREFIX := arm-none-eabi-
    CC := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
    AS := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)as
    LD := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)gcc
    OBJCOPY := $(DEVKITPRO)/devkitARM/bin/$(PREFIX)objcopy
    NDSTOOL := $(DEVKITPRO)/tools/bin/ndstool
    
    # Compiler flags
    CFLAGS := -g -Wall -O2 -mcpu=arm9e -mtune=arm9e -fomit-frame-pointer -ffast-math
    CFLAGS += -mthumb -mthumb-interwork
    CFLAGS += -I$(PLATFORM_DIR) -Icore -I$(DEVKITPRO)/libnds/include
    CFLAGS += -DARM9
    
    # Linker flags
    LDFLAGS := -specs=ds_arm9.specs -g -mthumb -mthumb-interwork
    LIBS := -L$(DEVKITPRO)/libnds/lib -lnds9
    
# Source files
PLATFORM_SRC := $(PLATFORM_DIR)/nds.c
OUTPUT := $(TARGET).nds
BUILD_DIR := $(BUILD)/nds

else ifeq ($(PLATFORM),ngc)
    # GameCube Configuration (placeholder for later)
    PREFIX := powerpc-eabi-
    CC := $(DEVKITPRO)/devkitPPC/bin/$(PREFIX)gcc
    
    # Compiler flags
    CFLAGS := -g -Wall -O2 -mcpu=750 -meabi -mhard-float
    CFLAGS += -I$(PLATFORM_DIR) -Icore -I$(DEVKITPRO)/libogc/include
    CFLAGS += -DPLATFORM_GAMECUBE -DGEKKO
    
    # Linker flags
    LDFLAGS := -g -Wl,-Map,$(TARGET).map
    LIBS := -L$(DEVKITPRO)/libogc/lib/gamecube -logc -lfat
    
    # Source files
    PLATFORM_SRC := $(PLATFORM_DIR)/ngc.c
    OUTPUT := $(TARGET).dol
    BUILD_DIR := $(BUILD)/ngc

else
    $(error Unknown platform: $(PLATFORM). Supported platforms: gba, nds, ngc)
endif

# All source files
SOURCES := main.c $(CORE_SRC) $(PLATFORM_SRC)

# Object files
OFILES := $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Default target (GBA only)
all: gba

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/platform

# Compile C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo compiling $(PLATFORM): $(notdir $<)
	@$(CC) $(CFLAGS) -c $< -o $@

# Platform-specific linking rules
ifeq ($(PLATFORM),gba)
# GBA linking
$(BUILD_DIR)/$(TARGET).elf: $(OFILES)
	@echo linking $(PLATFORM): $(notdir $@)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

$(TARGET).gba: $(BUILD_DIR)/$(TARGET).elf
	@echo built ... $(notdir $@)
	@$(OBJCOPY) -O binary $< $@
	@$(GBAFIX) $@ -t$(GAME_TITLE) -c$(GAME_CODE) -m$(MAKER_CODE)

else ifeq ($(PLATFORM),nds)
# NDS linking
$(BUILD_DIR)/$(TARGET).elf: $(OFILES)
	@echo linking $(PLATFORM): $(notdir $@)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

$(TARGET).nds: $(BUILD_DIR)/$(TARGET).elf
	@echo built ... $(notdir $@)
	@$(NDSTOOL) -c $@ -7 $(BUILD_DIR)/$(TARGET).elf

else ifeq ($(PLATFORM),ngc)
# GameCube linking
$(BUILD_DIR)/$(TARGET).elf: $(OFILES)
	@echo linking $(PLATFORM): $(notdir $@)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

$(TARGET).dol: $(BUILD_DIR)/$(TARGET).elf
	@echo built ... $(notdir $@)
	@$(ELF2DOL) $< $@

endif

# Clean
clean:
	@rm -rf $(BUILD)
	@rm -f $(TARGET).gba $(TARGET).nds $(TARGET).dol $(TARGET).map

# Clean specific platform
clean-$(PLATFORM):
	@rm -rf $(BUILD_DIR)
	@rm -f $(OUTPUT) $(TARGET).map

# Build all platforms (GBA only for now)
all-platforms: gba

gba: $(OUTPUT)

nds:
	@echo "NDS build not fully implemented yet"
	@$(MAKE) PLATFORM=nds || true

ngc:
	@echo "GameCube build not fully implemented yet"
	@$(MAKE) PLATFORM=ngc || true

.PHONY: all clean clean-$(PLATFORM) all-platforms gba nds ngc