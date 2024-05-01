// * Handles CPU data fetching.

#include <cpu.h>
#include <bus.h>
#include <emu.h>

// ===== Globals ===============================================================

extern cpuContext_t ctx;

// ===== Fetching data =========================================================

/**
 * Fetches data for the current instruction.
 */
void fetchData() {
    ctx.memoryDestination = 0;
    ctx.destinationIsMemory = false;

    if (ctx.currentInstruction == NULL) {
        return;  // Avoid segfaults
    }

    // Handle different instruction modalities
    switch (ctx.currentInstruction->mode) {
        // Implied - Nothing to read after this
        case AM_IMP:
            return;

        // Single register
        case AM_R:
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register1);
            return;

        // Register into register
        case AM_R_R:
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);

            return;

        // 8-bit bus data into register
        case AM_R_D8:
            ctx.fetchedData = readBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;

            return;

        // 16-bit bus data into register
        case AM_R_D16: {
            // Separated for cycle accuracy
            u16 lo = readBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            ctx.fetchedData = lo | (hi << 8);
            ctx.registers.pc += 2;

            return;
        }

        // 8-bit address into register
        case AM_R_A8: {
            ctx.fetchedData = readBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;
            return;
        }

        // 16-bit address into register
        case AM_R_A16: {
            // Separated for cycle accuracy
            u16 lo = readBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            u16 addr = lo | (hi << 8);

            ctx.registers.pc += 2;
            ctx.fetchedData = readBus(addr);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            return;
        }

        // Memory location into register
        case AM_R_MR: {
            u16 addr = readCPURegister(ctx.currentInstruction->register2);

            if (ctx.currentInstruction->register2 == RT_C) {
                addr |= 0xFF00;
            }

            ctx.fetchedData = readBus(addr);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading

            return;
        }

        // HL register into register, then increment
        case AM_R_HLI: {
            ctx.fetchedData =
                readBus(readCPURegister(ctx.currentInstruction->register2));
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            setCPURegister(RT_HL, readCPURegister(RT_HL) + 1);
            return;
        }

        // HL register into register, then decrement
        case AM_R_HLD: {
            ctx.fetchedData = readBus(ctx.currentInstruction->register2);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            setCPURegister(RT_HL, readCPURegister(RT_HL) - 1);
            return;
        }

        // Memory location (reference in register)
        case AM_MR: {
            ctx.memoryDestination =
                readCPURegister(ctx.currentInstruction->register1);
            ctx.destinationIsMemory = true;
            ctx.fetchedData =
                readBus(readCPURegister(ctx.currentInstruction->register1));
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            return;
        }

        // Register into memory location (reference in register)
        case AM_MR_R: {
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);
            ctx.memoryDestination =
                readCPURegister(ctx.currentInstruction->register1);
            ctx.destinationIsMemory = true;

            if (ctx.currentInstruction->register1 == RT_C) {
                ctx.memoryDestination |= 0xFF00;
            }

            return;
        }

        // 8-bit data into memory location (reference in register)
        case AM_MR_D8: {
            ctx.fetchedData = readBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;
            ctx.memoryDestination =
                readCPURegister(ctx.currentInstruction->register1);
            ctx.destinationIsMemory = true;
            return;
        }

        // Stack pointer into HL register, increment by R8
        case AM_HL_SPR: {
            ctx.fetchedData = readBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;
            return;
        }

        // Register into HL register, then increment
        case AM_HLI_R: {
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);
            ctx.memoryDestination =
                readCPURegister(ctx.currentInstruction->register1);
            ctx.destinationIsMemory = true;
            setCPURegister(RT_HL, readCPURegister(RT_HL) + 1);
        }

        // Register into HL register, then decrement
        case AM_HLD_R: {
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);
            ctx.memoryDestination =
                readCPURegister(ctx.currentInstruction->register1);
            ctx.destinationIsMemory = true;
            setCPURegister(RT_HL, readCPURegister(RT_HL) - 1);
        }

        // 8-bit data
        case AM_D8:
            ctx.fetchedData = readBus(ctx.registers.pc);
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.registers.pc++;
            return;

        // 16-bit data
        case AM_D16: {
            // Separated for cycle accuracy
            u16 lo = readBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            ctx.fetchedData = lo | (hi << 8);
            ctx.registers.pc += 2;

            return;
        }

        // Register into 16-bit address
        case AM_D16_R: {
            u16 lo = readBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            ctx.memoryDestination = lo | (hi << 8);
            ctx.destinationIsMemory = true;

            ctx.registers.pc += 2;
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);
            return;
        }

        // Register into 8-bit address
        case AM_A8_R: {
            ctx.memoryDestination = readBus(ctx.registers.pc) | 0xFF00;
            emulateCPUCycles(1);  // 1 CPU cycle for bus reading
            ctx.destinationIsMemory = true;
            ctx.registers.pc++;
            return;
        }

        // Register to 16-bit address
        case AM_A16_R: {
            // Separated for cycle accuracy
            u16 lo = readBus(ctx.registers.pc);
            emulateCPUCycles(1);
            u16 hi = readBus(ctx.registers.pc + 1);
            emulateCPUCycles(1);

            ctx.memoryDestination = lo | (hi << 8);
            ctx.destinationIsMemory = true;

            ctx.registers.pc += 2;
            ctx.fetchedData =
                readCPURegister(ctx.currentInstruction->register2);
            return;
        }

        default:
            printf("%sERR:%s Unknown addressing mode! %s%d%s (%s0x%02X%s)\n",
                   CRED, CRST, CYEL, ctx.currentInstruction->mode, CRST, CMAG,
                   ctx.currentOpcode, CRST);
            exit(EXIT_FAILURE);
    }
}
