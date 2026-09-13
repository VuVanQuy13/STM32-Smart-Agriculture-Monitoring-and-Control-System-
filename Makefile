GCC_DIR ?= D:/WorkPlace/CV/Embedded_Build_Makefile
CC := $(GCC_DIR)/bin/arm-none-eabi-gcc
OBJ_COPY := $(GCC_DIR)/bin/arm-none-eabi-objcopy
OUTPUT_DIR ?= Output
OPT ?= -O0

SRC_DIRS := Driver/Src App/Src Core Startup
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJ_PATHS := $(patsubst %.c,$(OUTPUT_DIR)/%.o,$(SRC_FILES))
CHIP_FLAGS := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CHIP_FLAGS) -std=gnu11 -ffreestanding $(OPT) -Wall -Wextra -Werror -MMD -MP -IDriver/Inc -IApp/Inc
LDFLAGS := $(CHIP_FLAGS) -nostdlib -T Linker/stm_ls.ld -Wl,-Map=$(OUTPUT_DIR)/makefile.map
BUILD_CONFIG := $(OUTPUT_DIR)/build-config.json

export STM32_BUILD_CC = $(CC)
export STM32_BUILD_CFLAGS = $(CFLAGS)
export STM32_BUILD_LDFLAGS = $(LDFLAGS)
export STM32_BUILD_OBJCOPY = $(OBJ_COPY)
export STM32_BUILD_SOURCES = $(SRC_FILES)

.PHONY: build clean print-% FORCE
build: $(OUTPUT_DIR)/makefile.hex

print-%:
	@echo $($*)

clean:
	$(RM) $(OBJ_PATHS) $(OBJ_PATHS:.o=.d) $(OUTPUT_DIR)/makefile.elf $(OUTPUT_DIR)/makefile.hex $(OUTPUT_DIR)/makefile.map $(BUILD_CONFIG)

FORCE:

$(BUILD_CONFIG): FORCE Tools/update_build_config.ps1 Makefile
	@powershell -NoProfile -ExecutionPolicy Bypass -File Tools/update_build_config.ps1 -Path "$(BUILD_CONFIG)"

$(OUTPUT_DIR)/%.o: %.c Makefile $(BUILD_CONFIG)
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(@D)' | Out-Null"
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/makefile.elf: $(OBJ_PATHS) Linker/stm_ls.ld Makefile $(BUILD_CONFIG)
	$(CC) $(LDFLAGS) $(OBJ_PATHS) -o $@

$(OUTPUT_DIR)/makefile.hex: $(OUTPUT_DIR)/makefile.elf
	$(OBJ_COPY) -O ihex $< $@

-include $(OBJ_PATHS:.o=.d)
