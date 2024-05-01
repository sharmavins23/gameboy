#pragma once

#include <common.h>

// Addressing modes for instructions
typedef enum {
    AM_IMP,  // Implied - Nothing to read after this

    AM_R,      // Single register
    AM_R_R,    // Register into register
    AM_R_D8,   // 8-bit bus data into register
    AM_R_D16,  // 16-bit bus data into register
    AM_R_A8,   // 8-bit address into register
    AM_R_A16,  // 16-bit address into register
    AM_R_MR,   // Memory location into register
    AM_R_HLI,  // HL register into register, then increment
    AM_R_HLD,  // HL register into register, then decrement

    AM_MR,     // Memory location (reference in register)
    AM_MR_R,   // Register into memory location (reference in register)
    AM_MR_D8,  // 8-bit data into memory location (reference in register)

    AM_HL_SPR,  // Stack pointer into HL register, increment by R8
    AM_HLI_R,   // Register into HL register, then increment
    AM_HLD_R,   // Register into HL register, then decrement

    AM_D8,     // 8-bit data
    AM_D16,    // 16-bit data
    AM_D16_R,  // Register into 16-bit address
    AM_A8_R,   // Register into 8-bit address
    AM_A16_R   // Register to 16-bit address

} addressingMode_t;

// Register types for instructions
typedef enum {
    RT_NONE,  // No register needed
    RT_A,     // 8-bit registers
    RT_F,
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_AF,  // 16-bit registers
    RT_BC,
    RT_DE,
    RT_HL,
    RT_SP,
    RT_PC
} registerType_t;

// Instruction types
typedef enum {
    IN_NONE,
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_RLCA,
    IN_ADD,
    IN_RRCA,
    IN_STOP,
    IN_RLA,
    IN_JR,
    IN_RRA,
    IN_DAA,
    IN_CPL,
    IN_SCF,
    IN_CCF,
    IN_HALT,
    IN_ADC,
    IN_SUB,
    IN_SBC,
    IN_AND,
    IN_XOR,
    IN_OR,
    IN_CP,
    IN_POP,
    IN_JP,
    IN_PUSH,
    IN_RET,
    IN_CB,
    IN_CALL,
    IN_RETI,
    IN_LDH,
    IN_JPHL,
    IN_DI,
    IN_EI,
    IN_RST,
    IN_ERR,
    // CB instructions...
    IN_RLC,
    IN_RRC,
    IN_RL,
    IN_RR,
    IN_SLA,
    IN_SRA,
    IN_SWAP,
    IN_SRL,
    IN_BIT,
    IN_RES,
    IN_SET
} instructionType_t;

// Condition types for instructions
typedef enum {
    CT_NONE,  // No conditions
    CT_NZ,    // If zero flag isn't set
    CT_Z,     // If zero flag is set
    CT_NC,    // If carry flag isn't set
    CT_C      // If carry flag is set
} conditionType_t;

// Instruction structure
typedef struct {
    instructionType_t type;
    addressingMode_t mode;
    registerType_t register1;
    registerType_t register2;
    conditionType_t cond;
    u8 param;
} instruction_t;

/**
 * Gets an instruction object from an opcode.
 *
 * @param opcode The opcode.
 * @return The instruction object.
 */
instruction_t *getInstructionFromOpcode(u8 opcode);

/**
 * Gets a human-readable instruction name from an instruction type.
 *
 * @param t The instruction object
 * @return The instruction name
 */
char *getInstructionName(instructionType_t instructionType);
