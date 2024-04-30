#pragma once

#include <common.h>

typedef enum {
    AM_IMP,    // Implied - Nothing to read after this
    AM_R_D16,  // 16-bit into register
    AM_R_R,    // Register into register
    AM_MR_R,   // Register into memory location (reference in register)
    AM_R,      // Single register
    AM_R_D8,   // 8-bit into register
    AM_R_MR,   // Memory location into register
    AM_R_HLI,
    AM_R_HLD,
    AM_HLI_R,
    AM_HLD_R,
    AM_R_A8,
    AM_A8_R,
    AM_HL_SPR,
    AM_D16,
    AM_D8,
    AM_D16_R,
    AM_MR_D8,
    AM_MR,
    AM_A16_R,
    AM_R_A16
} addr_mode;

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
} reg_type;

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
} in_type;

typedef enum {
    CT_NONE,  // No conditions
    CT_NZ,    // If zero flag isn't set
    CT_Z,     // If zero flag is set
    CT_NC,    // If carry flag isn't set
    CT_C      // If carry flag is set
} cond_type;

typedef struct {
    in_type type;
    addr_mode mode;
    reg_type reg_1;
    reg_type reg_2;
    cond_type cond;
    u8 param;
} instruction;

instruction *instruction_by_opcode(u8 opcode);

char *inst_name(in_type t);