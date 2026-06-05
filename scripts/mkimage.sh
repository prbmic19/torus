#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

TOPDIR="$1"
if [ -z "$TOPDIR" ]; then
    echo "TOPDIR variable undefined. Pass it to the command line." >&2
    exit 1
fi

ARCH="$2"
if [ -z "$ARCH" ]; then
    echo "ARCH variable undefined. Pass it to the command line." >&2
    exit 1
fi

ARCH_DIR="$TOPDIR/arch/$ARCH"
BUILD_DIR="$ARCH_DIR/build"

IMG="$ARCH_DIR/torus.img"

read_size_from_bin() {
    # Read 4 bytes at offset 6 (skip signature).
    od -An -tu4 -j6 -N4 "$1" | tr -d ' '
}

# For calculating sector count.
get_sector_count() {
    echo $((($1 + 511) / 512))
}

build_x86_image() {
    local boot_bin="$BUILD_DIR/boot.bin"
    local stage2_bin="$BUILD_DIR/stage2.bin"
    local kernel_bin="$BUILD_DIR/kernel.bin"

    local stage2_size=$(read_size_from_bin "$stage2_bin")
    local kernel_size=$(read_size_from_bin "$kernel_bin")

    local stage2_sectors=$(get_sector_count "$stage2_size")
    local kernel_sectors=$(get_sector_count "$kernel_size")

    # Sector 0    = boot
    # Sector 1..N = stage 2
    # Sector N..M = kernel

    local stage2_lba=1
    local kernel_lba=$((stage2_lba + stage2_sectors))

    local total_sectors=$((1 + stage2_sectors + kernel_sectors))

    dd if=/dev/zero of="$IMG" bs=512 count="$total_sectors" status=none

    dd if="$boot_bin" of="$IMG" conv=notrunc status=none
    dd if="$stage2_bin" of="$IMG" bs=512 seek="$stage2_lba" conv=notrunc status=none
    dd if="$kernel_bin" of="$IMG" bs=512 seek="$kernel_lba" conv=notrunc status=none
}

if [[ $ARCH == "x86" ]]; then
    build_x86_image
else
    echo "Architecture '$ARCH' is unsupported." >&2
    exit 1
fi
