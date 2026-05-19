# SPDX-License-Identifier: GPL-3.0-or-later 

include $(TOPDIR)/scripts/pretty_build.mk

ifeq ($(DR_CFLAGS),)
$(error DR_CFLAGS undefined)
endif

STAGE2_BUILD_DIR := $(BUILD_DIR)/arch/$(ARCH)/boot/stage2

# boot.o is handled separately.
OBJS = \
	$(STAGE2_BUILD_DIR)/boot_info.o \
	$(STAGE2_BUILD_DIR)/pmjump.o \
	$(STAGE2_BUILD_DIR)/pm.o \
	$(STAGE2_BUILD_DIR)/kjump.o

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

# Pass our kernel command line here for now.
CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	-nostdlib -nostartfiles -Wall -Wextra -Werror -I$(TOPDIR)/include \
	-I$(ARCH_DIR)/include -I/usr/include -std=gnu11 $(DR_CFLAGS) -MMD -MP \
	-DKERNEL_CMDLINE="\"$(KERNEL_CMDLINE)\""
ASFLAGS = -m32 -ffreestanding -I$(TOPDIR)/include -MMD -MP

ifeq ($(ARCH),x86)
CFLAGS += -mno-red-zone
endif

all: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/stage2.bin

## Bootloader

$(BUILD_DIR)/boot.elf: boot.S
	@mkdir -p $(dir $@)
	$(call pretty_build,AS,$(BUILD_DIR)/boot.o)
	@$(CC) $(ASFLAGS) -c $< -o $(BUILD_DIR)/boot.o
	$(call pretty_build,LD,$@)
	@$(LD) -m elf_i386 -e boot -Ttext 0x7c00 $(BUILD_DIR)/boot.o -o $@

$(BUILD_DIR)/boot.bin: $(BUILD_DIR)/boot.elf
	$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@

## Stage 2

$(BUILD_DIR)/stage2.elf: $(OBJS) 
	$(call pretty_build,LD,$@)
	@$(LD) -m elf_i386 -z noexecstack -T stage2/linker.ld $^ -o $@

$(BUILD_DIR)/stage2.bin: $(BUILD_DIR)/stage2.elf
	$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@

$(STAGE2_BUILD_DIR)/%.o: stage2/%.c
	@mkdir -p $(dir $@)
	$(call pretty_build,CC,$@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(STAGE2_BUILD_DIR)/%.o: stage2/%.S
	@mkdir -p $(dir $@)
	$(call pretty_build,AS,$@)
	@$(CC) $(ASFLAGS) -c $< -o $@

.PHONY: all
