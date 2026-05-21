# SPDX-License-Identifier: GPL-3.0-or-later

# Silence!
MAKEFLAGS += -s --no-print-directory

TOPDIR := $(CURDIR)
export TOPDIR

SUPPORTED_ARCHS := x86

ifeq ($(wildcard .config),)
ifeq ($(filter %config,$(MAKECMDGOALS)),)
$(error '.config' missing. Run 'make config' first)
endif
endif

-include .config

ifeq ($(filter %config,$(MAKECMDGOALS)),)
ifeq ($(ARCH),x86)
CROSS_PREFIX := x86_64-linux-gnu-
else
$(error Architecture '$(ARCH)' is unsupported)
endif
endif

# DR stands for debug/release.
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
export DR_CFLAGS

CC := $(CROSS_PREFIX)gcc
LD := $(CROSS_PREFIX)ld
OBJCOPY := $(CROSS_PREFIX)objcopy
export CC LD OBJCOPY

ARCH_DIR := $(TOPDIR)/arch/$(ARCH)
BUILD_DIR := $(ARCH_DIR)/build
export ARCH_DIR BUILD_DIR
IMG := $(ARCH_DIR)/torus.img

include scripts/pretty_build.mk

all: image

boot:
	@$(MAKE) -C $(ARCH_DIR)/boot

kernel:
	@$(MAKE) -C kernel

image: boot kernel
	@test -f scripts/mkimage.sh || (echo "'scripts/mkimage.sh' not found."; exit 1)
	@$(call pretty_build,MKIMAGE,$(call toprelpath,$(IMG)))
	@chmod +x scripts/mkimage.sh
	@./scripts/mkimage.sh $(TOPDIR) $(ARCH)

run: all
	$(call pretty_build,QEMU,$(ARCH_DIR)/torus.img)
	@which qemu-system-x86_64 > /dev/null || (echo "'qemu-system-x86_64' not found."; exit 1)
	@qemu-system-x86_64 -drive format=raw,file=$(IMG),if=ide -boot c

clean:
	$(call pretty_build,CLEAN,$(BUILD_DIR))
	$(call pretty_build,CLEAN,$(ARCH_DIR)/torus.img)
	@rm -rf $(BUILD_DIR) $(IMG)

purge: clean
	$(call pretty_build,CLEAN,.config .config.old)
	@rm -rf .config*

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
	@echo "  all         - Build all targets marked with '*'."
	@echo "* boot        - Build the bootloader."
	@echo "* kernel      - Build the kernel."
	@echo "* image       - Build the OS image."
	@echo ""
	@echo "Other targets:"
	@echo "  run         - Run the OS with QEMU."
	@echo "  clean       - Remove most generated files but keep config."
	@echo "  purge       - Remove all generated files."
	@echo "  listarch    - List all supported architectures."
	@echo "  config      - Update build config."
	@echo "  showconfig  - Display current build config."

.PHONY: all boot kernel image run clean purge listarch config showconfig
