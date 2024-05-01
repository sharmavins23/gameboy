#pragma once

#include <common.h>

// RAM context - Contains all RAM state
typedef struct {
    u8 wram[0x2000];  // Working RAM
    u8 hram[0x80];    // High RAM
} ramContext_t;

/**
 * Reads a byte from the given address in the working RAM.
 *
 * @param address The address to read from.
 * @return The byte read from the working RAM.
 */
u8 readWorkingRAM(u16 address);

/**
 * Writes a byte to the given address in the working RAM.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToWorkingRAM(u16 address, u8 value);

/**
 * Reads a byte from the given address in the high RAM.
 *
 * @param address The address to read from.
 * @return The byte read from the high RAM.
 */
u8 readHighRAM(u16 address);

/**
 * Writes a byte to the given address in the high RAM.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToHighRAM(u16 address, u8 value);