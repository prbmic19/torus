ARCH_INCLUDE := arch/x86/include

ARCH_CFLAGS_KERNEL_NO_INC := $(CFLAGS) -m64 -mno-red-zone -mgeneral-regs-only
ARCH_CFLAGS_KERNEL := $(ARCH_CFLAGS_KERNEL_NO_INC) -I$(ARCH_INCLUDE)
ARCH_CFLAGS_BOOT := $(CFLAGS) -I$(ARCH_INCLUDE) -m32 -mno-red-zone -mgeneral-regs-only
ARCH_ASFLAGS_KERNEL := $(ASFLAGS) -I$(ARCH_INCLUDE) -m64
ARCH_ASFLAGS_BOOT := $(ASFLAGS) -I$(ARCH_INCLUDE) -m32

BOOT_ELF := $(BUILD_DIR)/boot.elf
BOOT_BIN := $(BUILD_DIR)/boot.bin
STAGE2_ELF := $(BUILD_DIR)/stage2.elf
STAGE2_BIN := $(BUILD_DIR)/stage2.bin
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

IMG_DEPS := $(BOOT_BIN) $(STAGE2_BIN) $(KERNEL_BIN)

BOOT_OBJ := $(BUILD_DIR)/$(BOOT_DIR)/boot.o
STAGE2_OBJS := $(filter $(BUILD_DIR)/$(STAGE2_DIR)/%,$(OBJS))
KERNEL_OBJS := $(filter \
	$(BUILD_DIR)/$(ARCH_KERNEL_DIR)/% \
	$(BUILD_DIR)/kernel/% \
	$(BUILD_DIR)/drivers/% \
	$(BUILD_DIR)/lib/%,$(OBJS))

RUN_CMD := \
	$(call pretty_build,QEMU,$(IMG)); \
	which qemu-system-x86_64 > /dev/null || (echo "'qemu-system-x86_64' not found."; exit 1); \
	qemu-system-x86_64 -drive format=raw,file=$(IMG),if=ide -boot c -no-reboot -no-shutdown \
	-serial stdio

## Bootloader.

$(BOOT_ELF): $(BOOT_OBJ)
	@$(call pretty_build,LD,$@)
	@$(LD) -m elf_i386 -e boot -Ttext 0x7c00 $< -o $@

$(BOOT_BIN): $(BOOT_ELF)
	@$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@

$(STAGE2_ELF): $(STAGE2_OBJS)
	@$(call pretty_build,LD,$@)
	@$(LD) -m elf_i386 -z noexecstack -T $(STAGE2_DIR)/linker.ld $^ -o $@

$(STAGE2_BIN): $(STAGE2_ELF)
	@$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@

## Kernel.

$(KERNEL_ELF): $(KERNEL_OBJS)
	@$(call pretty_build,LD,$@)
	@$(LD) -m elf_x86_64 -z noexecstack -T $(ARCH_KERNEL_DIR)/linker.ld $^ -o $@

$(KERNEL_BIN): $(KERNEL_ELF)
	@$(call pretty_build,OBJCOPY,$@)
	@$(OBJCOPY) -O binary $< $@
