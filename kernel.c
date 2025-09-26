#include "common.h"
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

__attribute__((section(".text.boot"))) // Since OpenSBI jumps directly to 0x80200000 without knowing the entry point, the boot function needs to be placed here.
__attribute__((naked)) // Don’t generate unnecessary code before and after the function body which allows the function inline assembly code to be the exact function body.
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer to the end address of the stack area defined in the linker script
        "j kernel_main\n" // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the __stack_top address as stack_top
    );
}

void handle_trap(struct trap_frame *f) {
    uint32_t scause = READ_CSR(scause); // Cause behind the exception
    uint32_t stval = READ_CSR(stval); // Additional info about the exception
    uint32_t user_pc = READ_CSR(sepc); // Program counter at the point where the exception occurred

    // Right now, CPU enters a halted state because nothing is done to resume operations
    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc); // scause=00000002, stval=00000000, sepc=80200114 --> 00000002 (2) signifies an illegal instruction
}

/* Write data present in ch to debug console */
void sbi_console_putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, /* EID #0x01 */ 1);
}

__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        "csrw sscratch, sp\n" // Save the old stack pointer into sscratch at the point of an exception
        "addi sp, sp, -4 * 31\n"
        "sw ra,  4 * 0(sp)\n"
        "sw gp,  4 * 1(sp)\n"
        "sw tp,  4 * 2(sp)\n"
        "sw t0,  4 * 3(sp)\n"
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"
        "sw s1,  4 * 19(sp)\n"
        "sw s2,  4 * 20(sp)\n"
        "sw s3,  4 * 21(sp)\n"
        "sw s4,  4 * 22(sp)\n"
        "sw s5,  4 * 23(sp)\n"
        "sw s6,  4 * 24(sp)\n"
        "sw s7,  4 * 25(sp)\n"
        "sw s8,  4 * 26(sp)\n"
        "sw s9,  4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        "mv a0, sp\n" // Store stack pointer into a0
        "call handle_trap\n" // Call the C-handler

        "lw ra,  4 * 0(sp)\n" // Restore context
        "lw gp,  4 * 1(sp)\n"
        "lw tp,  4 * 2(sp)\n"
        "lw t0,  4 * 3(sp)\n"
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"
        "lw s1,  4 * 19(sp)\n"
        "lw s2,  4 * 20(sp)\n"
        "lw s3,  4 * 21(sp)\n"
        "lw s4,  4 * 22(sp)\n"
        "lw s5,  4 * 23(sp)\n"
        "lw s6,  4 * 24(sp)\n"
        "lw s7,  4 * 25(sp)\n"
        "lw s8,  4 * 26(sp)\n"
        "lw s9,  4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"
        "lw sp,  4 * 30(sp)\n"
        "sret\n"
    );
}

void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    WRITE_CSR(stvec, (uint32_t) kernel_entry); // Tell the CPU where the exception handler is located by writing to the stvec register
    __asm__ __volatile__("unimp"); // A “pseudo”-instruction that translates to csrrw x0, cycle, x0. More importantly, this instruction is guaranteed to “trap” the CPU, causing a PANIC. See more: https://github.com/riscv-non-isa/riscv-asm-manual/blob/main/src/asm-manual.adoc#instruction-aliases
    // llvm-addr2line -e kernel.elf 80200114 brings you here ^ --> convert a PC address into source file and line number

    const char *s = "\nHello, world!\n";

    for (int i = 0; s[i] != '\0'; i++) {
        sbi_console_putchar(s[i]);
    }

    printf("\nHello, %s!\n", "printf");
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

    // Placeholder idle-loop: do nothing till the CPU is interrupted
    for(;;) {
        // wfi --> Wait For Interrupt instruction
        // See: https://developer.arm.com/documentation/ddi0406/cb/Application-Level-Architecture/Instruction-Details/Alphabetical-list-of-instructions/WFI
        __asm__ __volatile__("wfi");
    }
}
