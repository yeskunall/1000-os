typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// __bss --> give me value at the 0th byte at __bss
// __bss[] --> give me the start address of __bss section
extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;

    while(n--)
        *p++ = c;

    return buf;
}

void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    for(;;);
}

__attribute__((section(".text.boot"))) // Since OpenSBI jumps directly to 0x80200000 without knowing the entry point, the boot function needs to be placed here
__attribute__((naked)) // Don’t generate unnecessary code before and after the function body which allows the function inline assembly code to be the exact function body
void bool(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer to the end address of the stack area defined in the linker script
        "j kernel_main\n" // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the __stack_top address as stack
    );
}
