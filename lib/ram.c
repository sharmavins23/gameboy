// * Handles RAM read/write functionality.

#include <ram.h>

// ===== Globals ===============================================================

// RAM context
static ramContext_t ctx;

// ===== RAM functionality =====================================================

/**
 * Reads a byte from the given address in the working RAM.
 *
 * @param address The address to read from.
 * @return The byte read from the working RAM.
 */
u8 readWorkingRAM(u16 address) {
    address -= 0xC000;
    return ctx.wram[address];
}

/**
 * Writes a byte to the given address in the working RAM.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToWorkingRAM(u16 address, u8 value) {
    address -= 0xC000;
    ctx.wram[address] = value;
}

/**
 * Reads a byte from the given address in the high RAM.
 *
 * @param address The address to read from.
 * @return The byte read from the high RAM.
 */
u8 readHighRAM(u16 address) {
    address -= 0xFF80;
    return ctx.hram[address];
}

/**
 * Writes a byte to the given address in the high RAM.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToHighRAM(u16 address, u8 value) {
    address -= 0xFF80;
    ctx.wram[address] = value;
}