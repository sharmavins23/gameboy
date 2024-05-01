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
    ctx.currentOpcode = readBus(ctx.registers.pc++);
    ctx.currentInstruction = getInstructionFromOpcode(ctx.currentOpcode);
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

        printf("PC %s0x%04X%s: %s%-7s%s (%s%02X%s %s%02X %02X%s) | ", CMAG, pc,
               CRST, CBLU, getInstructionName(ctx.currentInstruction->type),
               CRST, CCYN, ctx.currentOpcode, CRST, CMAG, readBus(pc + 1),
               readBus(pc + 2), CRST);
        printf(
            "AF=%s0x%04X%s BC=%s0x%04X%s DE=%s0x%04X%s HL=%s0x%04X%s "
            "SP=%s0x%04X%s\n",
            CMAG, readCPURegister(RT_AF), CRST, CMAG, readCPURegister(RT_BC),
            CRST, CMAG, readCPURegister(RT_DE), CRST, CMAG,
            readCPURegister(RT_HL), CRST, CMAG, readCPURegister(RT_SP), CRST);

        if (ctx.currentInstruction == NULL) {
            printf("%sERR:%s Unknown instruction encountered! %s0x%02X%s\n",
                   CRED, CRST, CMAG, ctx.currentOpcode, CRST);
            exit(EXIT_FAILURE);
        }

        execute();
    }
}
