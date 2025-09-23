/* ALIGN(4) directive ensures that the current address is adjusted to a 4-byte boundary */

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
    . += 128 * 1024; /* 128KB */
    __stack_top = .;
}
