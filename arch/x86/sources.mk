BOOT_DIR := arch/x86/boot
STAGE2_DIR := $(BOOT_DIR)/stage2
ARCH_KERNEL_DIR := arch/x86/kernel

CSRCS += \
	$(STAGE2_DIR)/pm.c \
	$(ARCH_KERNEL_DIR)/head.c \
	$(ARCH_KERNEL_DIR)/idt.c \
	$(ARCH_KERNEL_DIR)/irq.c \
	$(ARCH_KERNEL_DIR)/exception.c

ASSRCS += \
	$(BOOT_DIR)/boot.S \
	$(STAGE2_DIR)/boot_info.S \
	$(STAGE2_DIR)/pmjump.S \
	$(STAGE2_DIR)/kjump.S \
	$(ARCH_KERNEL_DIR)/head_s.S \
	$(ARCH_KERNEL_DIR)/isr_stubs.S
