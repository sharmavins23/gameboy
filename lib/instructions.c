// * Contains definitions for the instruction set and lookup functions.

#include <instructions.h>
#include <cpu.h>

// Map of instruction opcodes to their respective instruction object
instruction_t instructions[0x100] = {
    [0x00] = {IN_NOP, AM_IMP},       [0x05] = {IN_DEC, AM_R, RT_B},
    [0x0E] = {IN_LD, AM_R_D8, RT_C}, [0xAF] = {IN_XOR, AM_R, RT_A},
    [0xC3] = {IN_JP, AM_D16},        [0xF3] = {IN_DI}};

/**
 * Gets an instruction object from an opcode.
 *
 * @param opcode The opcode.
 * @return The instruction object.
 */
instruction_t *getInstructionFromOpcode(u8 opcode) {
    return &instructions[opcode];
}

char *instructionLookup[] = {
    "<NONE>", "NOP",     "LD",     "INC",    "DEC",    "RLCA",  "ADD",
    "RRCA",   "STOP",    "RLA",    "JR",     "RRA",    "DAA",   "CPL",
    "SCF",    "CCF",     "HALT",   "ADC",    "SUB",    "SBC",   "AND",
    "XOR",    "OR",      "CP",     "POP",    "JP",     "PUSH",  "RET",
    "CB",     "CALL",    "RETI",   "LDH",    "JPHL",   "DI",    "EI",
    "RST",    "IN_ERR",  "IN_RLC", "IN_RRC", "IN_RL",  "IN_RR", "IN_SLA",
    "IN_SRA", "IN_SWAP", "IN_SRL", "IN_BIT", "IN_RES", "IN_SET"};

/**
 * Gets a human-readable instruction name from an instruction type.
 *
 * @param t The instruction object
 * @return The instruction name
 */
char *getInstructionName(instructionType_t instructionType) {
    return instructionLookup[instructionType];
}