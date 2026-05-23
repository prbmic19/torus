# SPDX-License-Identifier: GPL-3.0-or-later

# Silence!
MAKEFLAGS += -s --no-print-directory

SUPPORTED_ARCHS := x86

ifeq ($(wildcard .config),) # No .config?
ifeq ($(filter %config,$(MAKECMDGOALS)),) # Target isn't config-related?
ifeq ($(filter help,$(MAKECMDGOALS)),) # Target isn't help?
$(error '.config' missing. Run 'make config' first)
endif
endif
endif

-include .config

ifeq ($(filter %config,$(MAKECMDGOALS)),) # Target isn't config-related?
ifeq ($(filter help,$(MAKECMDGOALS)),) # Target isn't help?
ifeq ($(filter $(ARCH),$(SUPPORTED_ARCHS)),) # Arch isn't supported?
$(error Architecture '$(ARCH)' is unsupported)
endif
endif
endif

CROSS_PREFIX_x86 := x86_64-linux-gnu-
CROSS_PREFIX := $(CROSS_PREFIX_$(ARCH))

CC      := $(CROSS_PREFIX)gcc
LD      := $(CROSS_PREFIX)ld
OBJCOPY := $(CROSS_PREFIX)objcopy

TOPDIR := $(CURDIR)

ARCH_DIR  := arch/$(ARCH)
BUILD_DIR := $(ARCH_DIR)/build

IMG := $(ARCH_DIR)/torus.img

# Ensure 'all' is the default target.
.DEFAULT_GOAL := all

## Flags.

# DR stands for debug/release.
# DEBUG_BUILD overrides OPT and DEBUG_SYMS.
ifeq ($(DEBUG_BUILD),y)
DR_CFLAGS = -ggdb -g3 -O0
else
DR_CFLAGS = -O$(OPT)
ifeq ($(DEBUG_SYMS),y)
DR_CFLAGS += -ggdb -g3
else
DR_CFLAGS += -s
endif
endif

# Common flags.
CFLAGS = -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	-Wall -Wextra -Werror -I$(TOPDIR)/include -std=gnu11 $(DR_CFLAGS) -MMD -MP
ASFLAGS = -I$(TOPDIR)/include -I$(ARCH_DIR)/include $(DR_CFLAGS) -MMD -MP

## Source files.

CSRCS  :=
ASSRCS :=
include kernel/sources.mk
-include $(ARCH_DIR)/sources.mk

## Object files.

OBJS := $(CSRCS:.c=.o) $(ASSRCS:.S=.o)
OBJS := $(addprefix $(BUILD_DIR)/,$(OBJS))

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

include scripts/pretty_build.mk
-include $(ARCH_DIR)/build.mk

# Check if an architecture's build.mk defined these flags.

ifneq ($(wildcard .config),)
ifneq ($(filter %config,$(MAKECMDGOALS)),)
ifeq ($(IMG_DEPS),)
$(error 'IMG_DEPS' undefined)
endif

ifeq ($(RUN_CMD),)
$(error 'RUN_CMD' undefined)
endif

ifeq ($(ARCH_CFLAGS_BOOT),)
$(error 'ARCH_CFLAGS_BOOT' undefined)
endif

ifeq ($(ARCH_CFLAGS_KERNEL),)
$(error 'ARCH_CFLAGS_KERNEL' undefined)
endif

ifeq ($(ARCH_ASFLAGS_BOOT),)
$(error 'ARCH_ASFLAGS_BOOT' undefined)
endif

ifeq ($(ARCH_ASFLAGS_KERNEL),)
$(error 'ARCH_ASFLAGS_KERNEL' undefined)
endif

ifeq ($(ARCH_INCLUDE),)
$(error 'ARCH_INCLUDE' undefined)
endif
endif
endif

all: $(IMG)

## Generic C.

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	@$(call pretty_build,CC,$@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/%.o: drivers/%.c
	@mkdir -p $(dir $@)
	@$(call pretty_build,CC,$@)
	@$(CC) $(CFLAGS) -I$(ARCH_INCLUDE) -c $< -o $@

## Arch-specific C (kernel and bootloader).

$(BUILD_DIR)/$(ARCH_DIR)/kernel/%.o: $(ARCH_DIR)/kernel/%.c
	@mkdir -p $(dir $@)
	@$(call pretty_build,CC,$@)
	@$(CC) $(ARCH_CFLAGS_KERNEL) -c $< -o $@

$(BUILD_DIR)/$(ARCH_DIR)/boot/%.o: $(ARCH_DIR)/boot/%.c
	@mkdir -p $(dir $@)
	@$(call pretty_build,CC,$@)
	@$(CC) $(ARCH_CFLAGS_BOOT) -c $< -o $@

## Assembly (kernel and bootloader) (arch-specific, of course).

$(BUILD_DIR)/$(ARCH_DIR)/kernel/%.o: $(ARCH_DIR)/kernel/%.S
	@mkdir -p $(dir $@)
	@$(call pretty_build,AS,$@)
	@$(CC) $(ARCH_ASFLAGS_KERNEL) -c $< -o $@

$(BUILD_DIR)/$(ARCH_DIR)/boot/%.o: $(ARCH_DIR)/boot/%.S
	@mkdir -p $(dir $@)
	@$(call pretty_build,AS,$@)
	@$(CC) $(ARCH_ASFLAGS_BOOT) -c $< -o $@

$(IMG): $(IMG_DEPS)
	@[ -f scripts/mkimage.sh ] || (echo "'scripts/mkimage.sh' not found."; exit 1)
	@$(call pretty_build,MKIMAGE,$(IMG))
	@chmod +x scripts/mkimage.sh
	@./scripts/mkimage.sh $(TOPDIR) $(ARCH)

## Other targets.

run: $(IMG)
	$(RUN_CMD)

clean:
	$(call pretty_build,CLEAN,$(BUILD_DIR))
	$(call pretty_build,CLEAN,$(IMG))
	@rm -rf $(BUILD_DIR) $(IMG)

purge: clean
	$(call pretty_build,CLEAN,.config .config.old)
	@rm -rf .config .config.old

# LOL
unmake: purge

listarch:
	@echo "List of supported architectures:"
	@echo "  $(SUPPORTED_ARCHS)"

config:
	@test -f scripts/config.sh || (echo "'scripts/config.sh' not found."; exit 1)
	$(call pretty_build,CALL,scripts/config.sh)
	@chmod +x scripts/config.sh
	@./scripts/config.sh $(TOPDIR)

showconfig:
	@test -f scripts/config.sh || (echo "'scripts/config.sh' not found."; exit 1)
	$(call pretty_build,CALL,scripts/config.sh)
	@chmod +x scripts/config.sh
	@./scripts/config.sh $(TOPDIR) show

help:
	@echo "Build targets:"
	@echo "  all         - Build the bootloader, kernel, and OS image."
	@echo ""
	@echo "Other targets:"
	@echo "  run         - Run the OS with QEMU."
	@echo "  clean       - Remove most generated files but keep config."
	@echo "  purge       - Remove all generated files."
	@echo "  listarch    - List all supported architectures."
	@echo "  config      - Update build config."
	@echo "  showconfig  - Display current build config."

.PHONY: all run clean purge unmake listarch config showconfig
