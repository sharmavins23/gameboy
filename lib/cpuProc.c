// * Processes CPU instructions.

#include <cpu.h>
#include <emu.h>

// ===== Helper functions ======================================================

/**
 * Checks the condition of the current instruction.
 *
 * @param ctx The CPU context.
 * @return Whether the condition is met.
 */
static bool checkCondition(cpuContext_t *ctx) {
    bool z = CPUFLAG_ZEROBIT(ctx);
    bool c = CPUFLAG_CARRYBIT(ctx);

    switch (ctx->currentInstruction->cond) {
        case CT_NONE:
            return true;
        case CT_C:
            return c;
        case CT_NC:
            return !c;
        case CT_Z:
            return z;
        case CT_NZ:
            return !z;
    }

    return false;  // Should never get here
}

/**
 * Sets the CPU flags. If a passed-in value is -1, it won't be modified.
 *
 * @param ctx The CPU context.
 * @param z The zero flag.
 * @param n The BCD N flag.
 * @param h The BCD H flag.
 * @param c The carry flag.
 */
void setCPUFlags(cpuContext_t *ctx, char z, char n, char h, char c) {
    // If we don't want to modify flags, we pass in flag = -1
    if (z != -1) {
        SETBIT(ctx->registers.f, 7, z);
    }
    if (n != -1) {
        SETBIT(ctx->registers.f, 6, n);
    }
    if (h != -1) {
        SETBIT(ctx->registers.f, 5, h);
    }
    if (c != -1) {
        SETBIT(ctx->registers.f, 4, c);
    }
}

// ===== Instruction processors ================================================

/**
 * Processor for instructions that don't have a processor.
 *
 * @param ctx The CPU context.
 */
static void procNone(cpuContext_t *ctx) {
    printf("%sERR:%s No processor for instruction %s0x%02X%s\n", CRED, CRST,
           CMAG, ctx->currentOpcode, CRST);
    exit(EXIT_FAILURE);
}

/**
 * Processor for NOP instructions.
 *
 * @param ctx The CPU context.
 */
static void procNOP(cpuContext_t *ctx) {
    // NOP doesn't do anything because it's a NOP
    return;
}

/**
 * Processor for LD instructions.
 * Loads data into a register.
 *
 * @param ctx The CPU context.
 */
static void procLD(cpuContext_t *ctx) { NO_IMPLEMENTATION("LD instruction"); }

/**
 * Processor for XOR instructions.
 * XORs the accumulator with the fetched data.
 *
 * @param ctx The CPU context.
 */
static void procXOR(cpuContext_t *ctx) {
    ctx->registers.a ^= ctx->fetchedData & 0xFF;
    setCPUFlags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

/**
 * Processor for JP instructions.
 * Jumps to a location if a condition is met.
 *
 * @param ctx The CPU context.
 */
static void procJP(cpuContext_t *ctx) {
    // If the condition matches...
    if (checkCondition(ctx)) {
        // Set program counter to the location of our fetched data
        ctx->registers.pc = ctx->fetchedData;
        emulateCPUCycles(1);  // Jumps are 1 cycle long
    }
}

/**
 * Processor for DI instructions.
 * Disables interrupts.
 *
 * @param ctx The CPU context.
 */
static void procDI(cpuContext_t *ctx) { ctx->masterInterruptEnabled = false; }

// ===== Instruction processor array ===========================================

// Array of function pointers for processing instructions
static IN_PROC processors[] = {
    [IN_NONE] = procNone,  // IN_NONE
    [IN_NOP] = procNOP,    // NOP inst
    [IN_LD] = procLD,      // Load inst
    [IN_XOR] = procXOR,    // XOR inst
    [IN_JP] = procJP,      // Jump inst
    [IN_DI] = procDI       // Sets master disabled
};

/**
 * Gets the processor for a given instruction type.
 *
 * @param type The instruction type.
 * @return The processor function pointer.
 */
IN_PROC getProcessorForInstructionType(instructionType_t type) {
    return processors[type];
}