// * Handles interrupts for the CPU.

#include <interrupts.h>
#include <cpu.h>
#include <stack.h>

// ===== Helper functions ======================================================

/**
 * Handles an interrupt by pushing the current program counter to the stack.
 * The program counter is then set to the interrupt handler address.
 *
 * @param ctx The CPU context.
 * @param address The address of the interrupt handler.
 */
void interruptHandler(cpuContext_t *ctx, u16 address) {
    pushStack16(ctx->registers.pc);
    ctx->registers.pc = address;
}

/**
 * Checks if an interrupt is enabled and if the interrupt flag is set.
 * If both are true, the interrupt is handled.
 *
 * @param ctx The CPU context.
 * @param address The address of the interrupt handler.
 * @param interruptType The type of interrupt.
 * @return Whether the interrupt was handled.
 */
bool checkInterrupt(cpuContext_t *ctx, u16 address,
                    interruptType_t interruptType) {
    if (ctx->interruptFlags & interruptType &&
        ctx->interruptEnableRegister & interruptType) {
        interruptHandler(ctx, address);
        ctx->interruptFlags &= ~interruptType;  // Turn off the flag
        ctx->halted = false;
        ctx->masterInterruptEnabled = false;
        return true;
    }

    return false;
}

// ===== Interrupt functions ===================================================

/**
 * Handles interrupts for the CPU.
 *
 * @param ctx The CPU context.
 */
void handleCPUInterrupt(cpuContext_t *ctx) {
    // For now, just check all of the interrupts
    if (checkInterrupt(ctx, 0x40, INT_VBLANK) ||
        checkInterrupt(ctx, 0x48, INT_LCDSTAT) ||
        checkInterrupt(ctx, 0x50, INT_TIMER) ||
        checkInterrupt(ctx, 0x58, INT_SERIAL) ||
        checkInterrupt(ctx, 0x60, INT_JOYPAD)) {
        // Note: We don't check multiple - The OR will shortcut
        return;
    }
}