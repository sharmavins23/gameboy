#pragma once

#include <common.h>

// Addressing modes for instructions
typedef enum {
    AM_IMP,     // Implied - Nothing to read after this
    AM_R_D16,   // 16-bit into register
    AM_R_R,     // Register into register
    AM_MR_R,    // Register into memory location (reference in register)
    AM_R,       // Single register
    AM_R_D8,    // 8-bit into register
    AM_R_MR,    // Memory location into register
    AM_R_HLI,   // HL register memory location into register
    AM_R_HLD,   // HL register data into register
    AM_HLI_R,   // Register into HL register memory location
    AM_HLD_R,   // Register into HL register data
    AM_R_A8,    // A register into register
    AM_A8_R,    // Register into A register
    AM_HL_SPR,  // Stack pointer into HL register
    AM_D16,     // 16-bit data
    AM_D8,      // 8-bit data
    AM_D16_R,   // Register into 16-bit data
    AM_MR_D8,   // 8-bit data into memory location (reference in register)
    AM_MR,      // Memory location (reference in register)
    AM_A16_R,
    AM_R_A16
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
    registerType_t reg_1;
    registerType_t reg_2;
    conditionType_t cond;
    u8 param;
} instruction_t;

instruction_t *getInstructionFromOpcode(u8 opcode);

char *getInstructionName(instructionType_t instructionType);