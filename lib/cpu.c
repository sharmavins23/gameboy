// * Emulates the Game Boy CPU (LR35902).

#include <cpu.h>
#include <bus.h>
#include <emu.h>
#include <interrupts.h>
#include <dbg.h>

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
    // Default value for registers
    *((short *)&ctx.registers.a) = 0xB001;
    *((short *)&ctx.registers.b) = 0x1300;
    *((short *)&ctx.registers.d) = 0xD800;
    *((short *)&ctx.registers.h) = 0x4D01;
    // Default value for stack pointer
    ctx.registers.sp = 0xFFFE;

    // Default values for interrupts
    ctx.masterInterruptEnabled = false;
    ctx.enablingIME = false;
    ctx.interruptEnableRegister = 0;
    ctx.interruptFlags = 0;
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

        char instruction[16];
        instructionToString(&ctx, instruction);
        printf("PC %s%08X%s: %s%-16s%s (%s%02X%s %s%02X %02X%s) | ", CMAG, pc,
               CRST, CBLU, instruction, CRST, CCYN, ctx.currentOpcode, CRST,
               CMAG, readBus(pc + 1), readBus(pc + 2), CRST);
        printf(
            "A=%s%02X%s BC=%s%02X%02X%s DE=%s%02X%02X%s HL=%s%02X%02X%s "
            "SP=%s%04X%s | ",
            CMAG, ctx.registers.a, CRST, CMAG, ctx.registers.b, ctx.registers.c,
            CRST, CMAG, ctx.registers.d, ctx.registers.e, CRST, CMAG,
            ctx.registers.h, ctx.registers.l, CRST, CMAG, ctx.registers.sp,
            CRST);
        char flags[5];
        sprintf(flags, "%c%c%c%c", BIT(ctx.registers.f, 7) ? 'Z' : '-',
                BIT(ctx.registers.f, 6) ? 'N' : '-',
                BIT(ctx.registers.f, 5) ? 'H' : '-',
                BIT(ctx.registers.f, 4) ? 'C' : '-');
        printf("F=%s%02X%s (%s%s%s) | ", CMAG, ctx.registers.f, CRST, CBLU,
               flags, CRST);
        // Also print emulator clock cycles
        printf("(t=%08lx)\n", getEMUContext()->ticks);

        if (ctx.currentInstruction == NULL) {
            printf("%sERR:%s Unknown instruction encountered! %s0x%02X%s\n",
                   CRED, CRST, CMAG, ctx.currentOpcode, CRST);
            exit(EXIT_FAILURE);
        }

        debugUpdate();
        debugPrint();

        execute();
    } else {
        // If the CPU is halted...
        emulateCPUCycles(1);  // Halting causes cycles to occur

        if (ctx.interruptFlags) {
            ctx.halted = false;
        }

        if (ctx.masterInterruptEnabled) {
            handleCPUInterrupt(&ctx);
            ctx.enablingIME = false;
        }

        if (ctx.enablingIME) {
            ctx.masterInterruptEnabled = true;
        }
    }
}
