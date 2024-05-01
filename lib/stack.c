// * Handles the CPU stack.

#include <stack.h>
#include <cpu.h>
#include <bus.h>

// ===== Stack manipulation functions ==========================================

/**
 * Pushes a byte onto the stack.
 *
 * @param data The data to push.
 */
void pushStack(u8 data) {
    cpuRegisters_t *registers = getCPURegisters();
    // Decrement the stack pointer
    registers->sp--;
    // Write the data to the bus
    writeBus(getCPURegisters()->sp, data);
}

/**
 * Pops a byte from the stack.
 *
 * @return The popped value.
 */
u8 popStack() { return readBus(getCPURegisters()->sp++); }

/**
 * Pushes a 16-bit value onto the stack.
 *
 * @param data The data to push.
 */
void pushStack16(u16 data) {
    pushStack((data >> 8) & 0xFF);
    pushStack(data & 0xFF);
}

/**
 * Pops a 16-bit value from the stack.
 *
 * @return The popped value.
 */
u16 popStack16() {
    u16 lo = popStack();
    u16 hi = popStack();

    return (hi << 8) | lo;
}