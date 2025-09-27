/* ALIGN(n) directive ensures that the current address is adjusted to an n-byte boundary */

/* Declare the entry point of the program */
ENTRY(boot)

SECTIONS {
    /* Base address */
    . = 0x80200000;

    .text :{
        KEEP(*(.text.boot));
        *(.text .text.*);
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    . = ALIGN(4);
    /* Advance the current address by 128KB */
    . += 128 * 1024;
    __stack_top = .;

    . = ALIGN(4096);
    __free_ram = .;
    /* Advance the current address by 64MB */
    . += 64 * 1024 * 1024;
    __free_ram_end = .;
}
