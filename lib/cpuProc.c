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
        ctx->registers.f = SETBIT(ctx->registers.f, 7, z);
    }
    if (n != -1) {
        ctx->registers.f = SETBIT(ctx->registers.f, 6, n);
    }
    if (h != -1) {
        ctx->registers.f = SETBIT(ctx->registers.f, 5, h);
    }
    if (c != -1) {
        ctx->registers.f = SETBIT(ctx->registers.f, 4, c);
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

/**
 * Checks if a register type is 16-bit.
 *
 * @param registerType The register to check.
 * @return Whether the register is 16-bit.
 */
static bool is16Bit(registerType_t registerType) {
    return registerType >= RT_AF;
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
 * Processor for INC instructions.
 * Increments a register.
 *
 * @param ctx The CPU context.
 */
static void procINC(cpuContext_t *ctx) {
    u16 value = readCPURegister(ctx->currentInstruction->register1) + 1;

    if (is16Bit(ctx->currentInstruction->register1)) {
        emulateCPUCycles(1);  // Need to add 1 extra cycle
    }

    // Special case for the HL register
    if (ctx->currentInstruction->register1 == RT_HL &&
        ctx->currentInstruction->mode == AM_MR) {
        value = readBus(readCPURegister(RT_HL)) + 1;
        value &= 0xFF;
        writeBus(readCPURegister(RT_HL), value);
    } else {
        setCPURegister(ctx->currentInstruction->register1, value);
        value = readCPURegister(ctx->currentInstruction->register1);  // Re-read
    }

    // If the opcode's bottom 2 are set, skip
    if ((ctx->currentOpcode & 0x03) == 0x03) {
        return;
    }

    setCPUFlags(ctx, value == 0, 0, (value & 0xF) == 0, -1);
}

/**
 * Processor for DEC instructions.
 * Decrements a register.
 *
 * @param ctx The CPU context.
 */
static void procDEC(cpuContext_t *ctx) {
    u16 value = readCPURegister(ctx->currentInstruction->register1) - 1;

    if (is16Bit(ctx->currentInstruction->register1)) {
        emulateCPUCycles(1);  // Need to add 1 extra cycle
    }

    // Special case for the HL register
    if (ctx->currentInstruction->register1 == RT_HL &&
        ctx->currentInstruction->mode == AM_MR) {
        value = readBus(readCPURegister(RT_HL)) - 1;
        writeBus(readCPURegister(RT_HL), value);
    } else {
        setCPURegister(ctx->currentInstruction->register1, value);
        value = readCPURegister(ctx->currentInstruction->register1);  // Re-read
    }

    // If the opcode's bottom 2 are set, skip
    if ((ctx->currentOpcode & 0x0B) == 0x0B) {
        return;
        printf("Returning early...");
    }

    setCPUFlags(ctx, value == 0, 1, (value & 0xF) == 0x0F, -1);
}

static void procRLCA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRLCA()"); }

/**
 * Processor for ADD instructions.
 * Adds the fetched data to a register.
 *
 * @param ctx The CPU context.
 */
static void procADD(cpuContext_t *ctx) {
    u32 value =
        readCPURegister(ctx->currentInstruction->register1) + ctx->fetchedData;

    // Set up basic flags
    int z = (value & 0xFF) == 0;
    int h = (readCPURegister(ctx->currentInstruction->register1) & 0xF) +
                (ctx->fetchedData & 0xF) >=
            0x10;
    int c = (int)(readCPURegister(ctx->currentInstruction->register1) & 0xFF) +
                (int)(ctx->fetchedData & 0xFF) >=
            0x100;

    // If 16 bit...
    if (is16Bit(ctx->currentInstruction->register1)) {
        emulateCPUCycles(1);  // Need to add 1 extra cycle
        z = -1;
        h = (readCPURegister(ctx->currentInstruction->register1) & 0xFFF) +
                (ctx->fetchedData & 0xFFF) >=
            0x1000;
        u32 n = ((u32)readCPURegister(ctx->currentInstruction->register1) +
                 (u32)ctx->fetchedData);
        c = n >= 0x10000;
    }

    // If SP...
    if (ctx->currentInstruction->register1 == RT_SP) {
        // For the stack pointer, fetchedData may be negative
        value = readCPURegister(ctx->currentInstruction->register1) +
                (char)ctx->fetchedData;
        z = 0;
        h = (readCPURegister(ctx->currentInstruction->register1) & 0xF) +
                (ctx->fetchedData & 0xF) >=
            0x10;
        c = (int)(readCPURegister(ctx->currentInstruction->register1) & 0xFF) +
                (int)(ctx->fetchedData & 0xFF) >
            0x100;
    }

    setCPURegister(ctx->currentInstruction->register1, value & 0xFFFF);
    setCPUFlags(ctx, z, 0, h, c);
}

static void procRRCA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRRCA()"); }

static void procSTOP(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSTOP()"); }

static void procRLA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRLA()"); }

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

static void procRRA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRRA()"); }

static void procDAA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procDAA()"); }

static void procCPL(cpuContext_t *ctx) { NO_IMPLEMENTATION("procCPL()"); }

static void procSCF(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSCF()"); }

static void procCCF(cpuContext_t *ctx) { NO_IMPLEMENTATION("procCCF()"); }

static void procHALT(cpuContext_t *ctx) { NO_IMPLEMENTATION("procHALT()"); }

/**
 * Processor for ADC instructions.
 * Adds the fetched data to the accumulator with carry.
 *
 * @param ctx The CPU context.
 */
static void procADC(cpuContext_t *ctx) {
    ctx->registers.a =
        (ctx->registers.a + ctx->fetchedData + CPUFLAG_CARRYBIT(ctx)) & 0xFF;

    setCPUFlags(
        ctx, ctx->registers.a == 0, 0,
        (ctx->registers.a & 0xF) + (ctx->fetchedData & 0xF) +
                CPUFLAG_CARRYBIT(ctx) >
            0xF,
        (ctx->registers.a + ctx->fetchedData + CPUFLAG_CARRYBIT(ctx)) > 0xFF);
}

/**
 * Processor for SUB instructions.
 * Subtracts the fetched data from a register.
 *
 * @param ctx The CPU context.
 */
static void procSUB(cpuContext_t *ctx) {
    u16 value =
        readCPURegister(ctx->currentInstruction->register1) - ctx->fetchedData;

    int z = value == 0;
    int h = ((int)readCPURegister(ctx->currentInstruction->register1) & 0xF) -
                ((int)ctx->fetchedData & 0xF) <
            0;
    int c = (int)readCPURegister(ctx->currentInstruction->register1) -
                (int)ctx->fetchedData <
            0;

    setCPURegister(ctx->currentInstruction->register1, value);
    setCPUFlags(ctx, z, 1, h, c);
}

static void procSBC(cpuContext_t *ctx) {
    u8 value = ctx->fetchedData + CPUFLAG_CARRYBIT(ctx);

    int z = readCPURegister(ctx->currentInstruction->register1) - value == 0;
    int h = ((int)readCPURegister(ctx->currentInstruction->register1) & 0xF) -
                ((int)ctx->fetchedData & 0xF) - ((int)CPUFLAG_CARRYBIT(ctx)) <
            0;
    int c = ((int)readCPURegister(ctx->currentInstruction->register1)) -
                ((int)ctx->fetchedData) - ((int)CPUFLAG_CARRYBIT(ctx)) <
            0;
    setCPURegister(ctx->currentInstruction->register1,
                   readCPURegister(ctx->currentInstruction->register1) - value);
    setCPUFlags(ctx, z, 1, h, c);
}

static void procAND(cpuContext_t *ctx) { NO_IMPLEMENTATION("procAND()"); }

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

static void procOR(cpuContext_t *ctx) { NO_IMPLEMENTATION("procOR()"); }

static void procCP(cpuContext_t *ctx) { NO_IMPLEMENTATION("procCP()"); }

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

static void procCB(cpuContext_t *ctx) { NO_IMPLEMENTATION("procCB()"); }

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

static void procJPHL(cpuContext_t *ctx) { NO_IMPLEMENTATION("procJPHL()"); }

/**
 * Processor for DI instructions.
 * Disables interrupts.
 *
 * @param ctx The CPU context.
 */
static void procDI(cpuContext_t *ctx) { ctx->masterInterruptEnabled = false; }

static void procEI(cpuContext_t *ctx) { NO_IMPLEMENTATION("procEI()"); }

static void procRST(cpuContext_t *ctx) {
    goToAddress(ctx, ctx->currentInstruction->param, true);
}

static void procERR(cpuContext_t *ctx) { NO_IMPLEMENTATION("procERR()"); }

static void procRLC(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRLC()"); }

static void procRRC(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRRC()"); }

static void procRL(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRL()"); }

static void procRR(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRR()"); }

static void procSLA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSLA()"); }

static void procSRA(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSRA()"); }

static void procSWAP(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSWAP()"); }

static void procSRL(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSRL()"); }

static void procBIT(cpuContext_t *ctx) { NO_IMPLEMENTATION("procBIT()"); }

static void procRES(cpuContext_t *ctx) { NO_IMPLEMENTATION("procRES()"); }

static void procSET(cpuContext_t *ctx) { NO_IMPLEMENTATION("procSET()"); }

// ===== Instruction processor array ===========================================

// Array of function pointers for processing instructions
static IN_PROC processors[] = {
    [IN_NONE] = procNone, [IN_NOP] = procNOP,   [IN_LD] = procLD,
    [IN_INC] = procINC,   [IN_DEC] = procDEC,   [IN_RLCA] = procRLCA,
    [IN_ADD] = procADD,   [IN_RRCA] = procRRCA, [IN_STOP] = procSTOP,
    [IN_RLA] = procRLA,   [IN_JR] = procJR,     [IN_RRA] = procRRA,
    [IN_DAA] = procDAA,   [IN_CPL] = procCPL,   [IN_SCF] = procSCF,
    [IN_CCF] = procCCF,   [IN_HALT] = procHALT, [IN_ADC] = procADC,
    [IN_SUB] = procSUB,   [IN_SBC] = procSBC,   [IN_AND] = procAND,
    [IN_XOR] = procXOR,   [IN_OR] = procOR,     [IN_CP] = procCP,
    [IN_POP] = procPOP,   [IN_JP] = procJP,     [IN_PUSH] = procPUSH,
    [IN_RET] = procRET,   [IN_CB] = procCB,     [IN_CALL] = procCALL,
    [IN_RETI] = procRETI, [IN_LDH] = procLDH,   [IN_JPHL] = procJPHL,
    [IN_DI] = procDI,     [IN_EI] = procEI,     [IN_RST] = procRST,
    [IN_ERR] = procERR,   [IN_RLC] = procRLC,   [IN_RRC] = procRRC,
    [IN_RL] = procRL,     [IN_RR] = procRR,     [IN_SLA] = procSLA,
    [IN_SRA] = procSRA,   [IN_SWAP] = procSWAP, [IN_SRL] = procSRL,
    [IN_BIT] = procBIT,   [IN_RES] = procRES,   [IN_SET] = procSET};

/**
 * Gets the processor for a given instruction type.
 *
 * @param type The instruction type.
 * @return The processor function pointer.
 */
IN_PROC getProcessorForInstructionType(instructionType_t type) {
    return processors[type];
}