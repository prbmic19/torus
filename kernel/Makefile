# SPDX-License-Identifier: GPL-3.0-or-later

include ../scripts/pretty_build.mk

ifeq ($(DR_CFLAGS),)
$(error DR_CFLAGS undefined)
endif

KERNEL_BUILD_DIR := $(BUILD_DIR)/kernel
ARCH_KERNEL_BUILD_DIR = $(BUILD_DIR)/arch/$(ARCH)/kernel

OBJS = \
	$(KERNEL_BUILD_DIR)/main.o \
	$(ARCH_KERNEL_BUILD_DIR)/entry.o

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

CFLAGS = -m64 -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles \
	-Wall -Wextra -Werror -I$(TOPDIR)/include -I/usr/include -std=gnu11 \
	$(DR_CFLAGS) -MMD -MP
ASFLAGS = -m64 -ffreestanding -I$(TOPDIR)/include $(DR_CFLAGS) -MMD -MP

ifeq ($(ARCH),x86)
CFLAGS += -mno-red-zone
endif

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.elf: $(OBJS)
	@mkdir -p $(dir $@)
	$(call pretty_build,LD,$@)
	@$(LD) -m elf_x86_64 -z noexecstack -T linker.ld $^ -o $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@

$(KERNEL_BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(call pretty_build,CC,$@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_KERNEL_BUILD_DIR)/%.o: $(ARCH_DIR)/kernel/%.c
	@mkdir -p $(dir $@)
	$(call pretty_build,CC,$@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_KERNEL_BUILD_DIR)/%.o: $(ARCH_DIR)/kernel/%.S
	@mkdir -p $(dir $@)
	$(call pretty_build,AS,$@)
	@$(CC) $(ASFLAGS) -c $< -o $@

.PHONY: all
