#!/bin/bash

set -eux

# command -v qemu-system-riscv32
QEMU_BIN=qemu-system-riscv32

# Path to clang and compiler flags
# command -v clang
CC=/opt/homebrew/opt/llvm/bin/clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib -fuse-ld=lld"

# Build the kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c common.c

$QEMU_BIN -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf
