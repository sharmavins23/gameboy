// * Handles reading and writing of information to the shared bus.

#include <bus.h>
#include <cart.h>

// * Memory map
// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

// ===== Bus functions =========================================================

/**
 * Reads a byte from the bus at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the bus.
 */
u8 readFromBus(u16 address) {
    // For now, only read from ROM
    if (address < 0x8000) {
        return readCartridge(address);
    }

    NO_IMPLEMENTATION("readFromBus() outside ROM");
}

/**
 * Writes a byte to the bus at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToBus(u16 address, u8 value) {
    if (address < 0x8000) {
        // ROM Data
        writeToCartridge(address, value);
        return;
    }

    NO_IMPLEMENTATION("writeToBus() outside ROM");
}