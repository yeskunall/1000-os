#include "kernel.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// __bss --> give me value at the 0th byte at __bss
// __bss[] --> give me the start address of __bss section
extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
    register long a0 __asm__("a0") = arg0; // Place the value of arg0 into a0
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    // An ECALL is used as the control transfer instruction between the supervisor and the SEE. When this is called, the CPU’s execution mode switches from kernel mode (S-Mode) to OpenSBI mode (M-Mode), and OpenSBI’s processing handler is invoked. Once it’s done, it switches back to kernel mode, and execution resumes after the ECALL instruction.
    __asm__ __volatile__(
        "ecall" :
        // All registers except a0 & a1 must be preserved across an SBI call by the callee.
        "=r"(a0), "=r"(a1) :
        "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) :
        "memory"
    );

    // SBI functions must return a pair of values in a0 and a1, with a0 returning an error code.
    return (struct sbiret){.error = a0, .value = a1};
}

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;

    while(n--)
        *p++ = c;

    return buf;
}

/* Write data present in ch to debug console */
void sbi_console_putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, /* EID #0x01 */ 1);
}

__attribute__((section(".text.boot"))) // Since OpenSBI jumps directly to 0x80200000 without knowing the entry point, the boot function needs to be placed here.
__attribute__((naked)) // Don’t generate unnecessary code before and after the function body which allows the function inline assembly code to be the exact function body.
void bool(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer to the end address of the stack area defined in the linker script
        "j kernel_main\n" // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the __stack_top address as stack
    );
}

void kernel_main(void) {
    // memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    const char *s = "\nHello, world!\n";

    for (int i = 0; s[i] != '\0'; i++) {
        sbi_console_putchar(s[i]);
    }

    // for(;;);

    // Placeholder idle-loop: do nothing till the CPU is interrupted
    for(;;) {
        // wfi --> Wait For Interrupt instruction
        // See: https://developer.arm.com/documentation/ddi0406/cb/Application-Level-Architecture/Instruction-Details/Alphabetical-list-of-instructions/WFI
        __asm__ __volatile__("wfi");
    }
}
