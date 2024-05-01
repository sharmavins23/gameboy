// * Emulates the Game Boy CPU (LR35902).

#include <cpu.h>
#include <bus.h>
#include <emu.h>

// ===== Globals ===============================================================

// The CPU context object - contains all CPU state
cpuContext_t ctx = {0};

// ===== Helper functions ======================================================

/**
 * Fetches the next instruction from memory.
 */
static void fetchInstruction() {
    ctx.currentOpcode = readFromBus(ctx.registers.pc++);
    ctx.currentInstruction = getInstructionFromOpcode(ctx.currentOpcode);
}

/**
 * Fetches data for the current instruction.
 */
static void fetchData() {
    ctx.memoryDestination = 0;
    ctx.destinationIsMemory = false;

    if (ctx.currentInstruction == NULL) {
        return;  // Avoid segfaults
    }

    switch (ctx.currentInstruction->mode) {
        case AM_IMP:  // Implied mode

            return;

        case AM_R:  // Register
            ctx.fetchedData = readCPURegister(ctx.currentInstruction->reg_1);

            return;

        case AM_R_D8:
            ctx.fetchedData = readFromBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;

            return;

        case AM_D16: {
            u16 lo = readFromBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readFromBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            ctx.fetchedData = lo | (hi << 8);
            ctx.registers.pc += 2;

            return;
        }

        default:
            printf("%sERR:%s Unknown addressing mode! %s%d%s (%s0x%02X%s)\n",
                   CRED, CRST, CYEL, ctx.currentInstruction->mode, CRST, CMAG,
                   ctx.currentOpcode, CRST);
            exit(EXIT_FAILURE);
    }
}

/**
 * Executes the current instruction.
 */
static void execute() {
    IN_PROC proc = getProcessorForInstructionType(ctx.currentInstruction->type);

    if (!proc) {
        NO_IMPLEMENTATION("execute() without processor");
    }

    proc(&ctx);
}

// ===== CPU functions =========================================================

/**
 * Initializes the CPU.
 */
void initializeCPU() {
    // Set the program counter to the entrypoint
    ctx.registers.pc = 0x100;
    // Default value for A register
    ctx.registers.a = 0x01;
}

/**
 * Steps the CPU by one instruction.
 */
void stepCPU() {
    // If the CPU is running, fetch an instruction
    if (!ctx.halted) {
        u16 pc = ctx.registers.pc;

        fetchInstruction();
        fetchData();

        printf(
            "%s0x%04X%s: %s%-7s%s (%s%02X%s %s%02X %02X%s) A: %02X B: %02X C: "
            "%02X\n",
            CMAG, pc, CRST, CBLU,
            getInstructionName(ctx.currentInstruction->type), CRST, CCYN,
            ctx.currentOpcode, CRST, CMAG, readFromBus(pc + 1),
            readFromBus(pc + 2), CRST, ctx.registers.a, ctx.registers.b,
            ctx.registers.c);

        if (ctx.currentInstruction == NULL) {
            printf("%sERR:%s Unknown instruction encountered! %s0x%02X%s\n",
                   CRED, CRST, CMAG, ctx.currentOpcode, CRST);
            exit(EXIT_FAILURE);
        }

        execute();
    }
}
