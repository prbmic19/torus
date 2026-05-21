# SPDX-License-Identifier: GPL-3.0-or-later

include ../scripts/pretty_build.mk

ifeq ($(DR_CFLAGS),)
$(error DR_CFLAGS undefined)
endif

KERNEL_BUILD_DIR := $(BUILD_DIR)/kernel
ARCH_KERNEL_BUILD_DIR := $(BUILD_DIR)/arch/$(ARCH)/kernel
DRIVERS_BUILD_DIR := $(BUILD_DIR)/drivers

OBJS = \
	$(KERNEL_BUILD_DIR)/main.o \
	$(KERNEL_BUILD_DIR)/irq.o \
	$(KERNEL_BUILD_DIR)/timer.o \
	$(DRIVERS_BUILD_DIR)/pic.o \
	$(DRIVERS_BUILD_DIR)/pit.o

ifeq ($(ARCH),x86)
ARCH_OBJS = \
	$(ARCH_KERNEL_BUILD_DIR)/head_s.o \
	$(ARCH_KERNEL_BUILD_DIR)/head.o \
	$(ARCH_KERNEL_BUILD_DIR)/idt.o \
	$(ARCH_KERNEL_BUILD_DIR)/exception.o \
	$(ARCH_KERNEL_BUILD_DIR)/irq.o \
	$(ARCH_KERNEL_BUILD_DIR)/isr_stubs.o
endif

ALL_OBJS = $(OBJS) $(ARCH_OBJS)

DEPS := $(ALL_OBJS:.o=.d)
-include $(DEPS)

CFLAGS = -m64 -ffreestanding -fno-pic -fno-pie -nostdlib -Wall -Wextra -Werror \
	-I$(TOPDIR)/include -std=gnu11 $(DR_CFLAGS) -MMD -MP
ARCH_CFLAGS = $(CFLAGS) -I$(ARCH_DIR)/include
ARCH_ASFLAGS = -m64 -ffreestanding -I$(TOPDIR)/include -I$(ARCH_DIR)/include \
	$(DR_CFLAGS) -MMD -MP

ifeq ($(ARCH),x86)
CFLAGS += -mno-red-zone -mgeneral-regs-only
endif

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.elf: $(ALL_OBJS)
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
	@$(CC) $(ARCH_CFLAGS) -c $< -o $@

$(ARCH_KERNEL_BUILD_DIR)/%.o: $(ARCH_DIR)/kernel/%.S
	@mkdir -p $(dir $@)
	$(call pretty_build,AS,$@)
	@$(CC) $(ARCH_ASFLAGS) -c $< -o $@

$(DRIVERS_BUILD_DIR)/%.o: $(TOPDIR)/drivers/%.c
	@mkdir -p $(dir $@)
	$(call pretty_build,CC,$@)
	@$(CC) $(ARCH_CFLAGS) -c $< -o $@

.PHONY: all
