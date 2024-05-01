#pragma once

#include <common.h>
#include <instructions.h>

// CPU register structure - Contains all registers and their values
typedef struct {
    u8 a;    // Accumulator
    u8 f;    // Flags
    u8 b;    // General purpose register
    u8 c;    // Combines with B for BC register
    u8 d;    // General purpose register
    u8 e;    // Combines with D for DE register
    u8 h;    // General purpose register
    u8 l;    // Combines with H for HL register
    u16 pc;  // Program counter
    u16 sp;  // Stack pointer
} cpuRegisters_t;

// CPU context structure - Contains all CPU state
typedef struct {
    cpuRegisters_t registers;  // Registers and their values
    u16 fetchedData;  // Current data fetched from instruction (immediate)

    u16 memoryDestination;     // Memory destination for current processing
    bool destinationIsMemory;  // Is the destination a memory location?

    u8 currentOpcode;                   // Current instruction opcode
    instruction_t *currentInstruction;  // Current instruction

    bool halted;    // Is the CPU halted?
    bool stepping;  // Stepping mode (DBG)

    bool masterInterruptEnabled;  // Whether interrupts are enabled or disabled
    u8 interruptEnableRegister;   // Interrupt enable register
} cpuContext_t;

// Function pointer for instruction processing
typedef void (*IN_PROC)(cpuContext_t *);

// ===== Bit functions =========================================================

/**
 * Gets the zero bit from a CPU flag.
 *
 * @param ctx The CPU context.
 * @return The zero bit.
 */
static inline bool CPUFLAG_ZEROBIT(cpuContext_t *ctx) {
    return BIT(ctx->registers.f, 7);
}

/**
 * Gets the carry bit from a CPU flag.
 *
 * @param ctx The CPU context.
 * @return The carry bit.
 */
static inline bool CPUFLAG_CARRYBIT(cpuContext_t *ctx) {
    return BIT(ctx->registers.f, 4);
}

// ===== CPU fetch functions ===================================================

/**
 * Fetches data for the current instruction.
 */
void fetchData();

// ===== CPU processor functions ===============================================

/**
 * Gets the processor for a given instruction type.
 *
 * @param type The instruction type.
 * @return The processor function pointer.
 */
IN_PROC getProcessorForInstructionType(instructionType_t type);

// ===== CPU utility functions =================================================

/**
 * Reads a CPU register.
 *
 * @param registerType The register type.
 */
u16 readCPURegister(registerType_t registerType);

/**
 * Writes a value to a CPU register.
 *
 * @param registerType The register type.
 * @param val The value to write.
 */
void setCPURegister(registerType_t registerType, u16 value);

/**
 * Reads the CPU Interrupt Enable (IE) register.
 *
 * @return The value of the IE register.
 */
u8 readCPUIERegister();

/**
 * Writes a value to the CPU Interrupt Enable (IE) register.
 *
 * @param value The value to write.
 */
void setCPUIERegister(u8 value);

// ===== CPU functions =========================================================

/**
 * Initializes the CPU.
 */
void initializeCPU();

/**
 * Steps the CPU by one instruction.
 */
void stepCPU();
