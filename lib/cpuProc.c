// * Processes CPU instructions.

#include <cpu.h>
#include <emu.h>
#include <bus.h>
#include <stack.h>

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

/**
 * Pushes the program counter to the stack and jumps to an address.
 * Generic call for other jumping instructions.
 *
 * @param ctx The CPU context.
 * @param address The address to jump to.
 * @param pushPC Whether to push the program counter to the stack.
 */
static void goToAddress(cpuContext_t *ctx, u16 address, bool pushPC) {
    // If the condition matches...
    if (checkCondition(ctx)) {
        // If pushPC is set, we want to push the PC
        if (pushPC) {
            pushStack16(ctx->registers.pc);
            emulateCPUCycles(2);  // 2 cycles for pushing to stack
        }

        // Set program counter to the location of our address
        ctx->registers.pc = address;
        emulateCPUCycles(1);  // Jumps are 1 cycle long
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
static void procLD(cpuContext_t *ctx) {
    if (ctx->destinationIsMemory) {
        // If a 16-bit register...
        if (ctx->currentInstruction->register2 >= RT_AF) {
            writeBus16(ctx->memoryDestination, ctx->fetchedData);
            // ! Why 1 cycle and not 2?
            emulateCPUCycles(1);  // 1 cycle for writing to bus
        } else {
            writeBus(ctx->memoryDestination, ctx->fetchedData);
            emulateCPUCycles(1);  // 1 cycle for writing to bus
        }
    }

    if (ctx->currentInstruction->mode == AM_HL_SPR) {
        u8 hflag = (readCPURegister(ctx->currentInstruction->register2) & 0xF) +
                       (ctx->fetchedData & 0xF) >=
                   0x10;
        u8 cflag =
            (readCPURegister(ctx->currentInstruction->register2) & 0xFF) +
                (ctx->fetchedData & 0xF) >=
            0x100;

        setCPUFlags(ctx, 0, 0, hflag, cflag);
        setCPURegister(ctx->currentInstruction->register1,
                       readCPURegister(ctx->currentInstruction->register2) +
                           (char)ctx->fetchedData);
    }

    setCPURegister(ctx->currentInstruction->register1, ctx->fetchedData);
}

/**
 * Processor for JR instructions.
 * Jumps to a location relative to the current program counter.
 *
 * @param ctx The CPU context.
 */
static void procJR(cpuContext_t *ctx) {
    char rel = (char)(ctx->fetchedData & 0xFF);
    u16 addr = ctx->registers.pc + rel;
    goToAddress(ctx, addr, false);
}

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
 * Processor for POP instructions.
 * Pops a value from the stack into a register.
 *
 * @param ctx The CPU context.
 */
static void procPOP(cpuContext_t *ctx) {
    // Separated for cycle accuracy
    u16 lo = popStack();
    emulateCPUCycles(1);  // 1 cycle for popping from stack
    u16 hi = popStack();
    emulateCPUCycles(1);  // 1 cycle for popping from stack

    u16 data = (hi << 8) | lo;

    // ! This may be redundant
    setCPURegister(ctx->currentInstruction->register1, data);

    if (ctx->currentInstruction->register1 == RT_AF) {
        setCPURegister(ctx->currentInstruction->register1, data & 0xFFF0);
    }
}

/**
 * Processor for JP instructions.
 * Jumps to a location if a condition is met.
 *
 * @param ctx The CPU context.
 */
static void procJP(cpuContext_t *ctx) {
    goToAddress(ctx, ctx->fetchedData, false);
}

/**
 * Processor for PUSH instructions.
 * Pushes a value from a register onto the stack.
 *
 * @param ctx The CPU context.
 */
static void procPUSH(cpuContext_t *ctx) {
    // Separated for cycle accuracy
    u16 hi = (readCPURegister(ctx->currentInstruction->register1) >> 8) & 0xFF;
    emulateCPUCycles(1);  // 1 cycle for reading from register
    pushStack(hi);

    u16 lo = readCPURegister(ctx->currentInstruction->register2) & 0xFF;
    emulateCPUCycles(1);  // 1 cycle for reading from register
    pushStack(lo);

    // ! Is this extra necessary?
    emulateCPUCycles(1);  // 1 cycle for pushing to stack
}

/**
 * Processor for RET instructions.
 * Returns from a subroutine if a condition is met.
 *
 * @param ctx The CPU context.
 */
static void procRET(cpuContext_t *ctx) {
    if (ctx->currentInstruction->cond != CT_NONE) {
        // 1 cycle for command execution
        emulateCPUCycles(1);  // Have to hang here
    }

    if (checkCondition(ctx)) {
        // Separated for cycle accuracy
        u16 lo = popStack();
        emulateCPUCycles(1);
        u16 hi = popStack();
        emulateCPUCycles(1);

        u16 addr = (hi << 8) | lo;
        ctx->registers.pc = addr;

        emulateCPUCycles(1);  // 1 cycle for checking condition
    }
}

/**
 * Processor for CALL instructions.
 * Calls a subroutine if a condition is met.
 *
 * @param ctx The CPU context.
 */
static void procCALL(cpuContext_t *ctx) {
    goToAddress(ctx, ctx->fetchedData, true);
}

/**
 * Processor for RETI instructions.
 * Returns from an interrupt.
 *
 * @param ctx The CPU context.
 */
static void procRETI(cpuContext_t *ctx) {
    // Re-enable master interrupt flag
    ctx->masterInterruptEnabled = true;
    procRET(ctx);
}

/**
 * Processor for LDH instructions.
 * Loads data into a register or memory location.
 *
 * @param ctx The CPU context.
 */
static void procLDH(cpuContext_t *ctx) {
    if (ctx->currentInstruction->register1 == RT_A) {
        setCPURegister(ctx->currentInstruction->register1,
                       readBus(0xFF00 | ctx->fetchedData));

    } else {
        writeBus(ctx->memoryDestination, ctx->registers.a);
    }
    emulateCPUCycles(1);  // 1 cycle for bus reading
}

/**
 * Processor for DI instructions.
 * Disables interrupts.
 *
 * @param ctx The CPU context.
 */
static void procDI(cpuContext_t *ctx) { ctx->masterInterruptEnabled = false; }

static void procRST(cpuContext_t *ctx) {
    goToAddress(ctx, ctx->currentInstruction->param, true);
}

// ===== Instruction processor array ===========================================

// Array of function pointers for processing instructions
static IN_PROC processors[] = {
    [IN_NONE] = procNone,  // IN_NONE
    [IN_NOP] = procNOP,    // NOP inst
    [IN_LD] = procLD,      // Load inst
    [IN_JR] = procJR,      // Jump relative inst
    [IN_XOR] = procXOR,    // XOR inst
    [IN_POP] = procPOP,    // Pop inst
    [IN_JP] = procJP,      // Jump inst
    [IN_PUSH] = procPUSH,  // Push inst
    [IN_RET] = procRET,    // Return inst
    [IN_CALL] = procCALL,  // Call inst
    [IN_RETI] = procRETI,  // Return from interrupt
    [IN_LDH] = procLDH,    // Load high inst
    [IN_DI] = procDI,      // Sets master disabled
    [IN_RST] = procRST     // Reset inst
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