// * Handles reading and writing of information to the shared bus.

#include <bus.h>
#include <cart.h>
#include <cpu.h>
#include <ram.h>

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
// 0xFF80 - 0xFFFE : Zero Page (high RAM)

// ===== Bus functions =========================================================

/**
 * Reads a byte from the bus at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the bus.
 */
u8 readBus(u16 address) {
    if (address < 0x8000) {  // ROM data
        return readCartridge(address);
    } else if (address < 0xA000) {  // Character/Map data
        NO_IMPLEMENTATION("readBus() for Character/Map data");
    } else if (address < 0xC000) {  // Cartridge RAM
        return readCartridge(address);
    } else if (address < 0xE000) {  // Working RAM
        return readWorkingRAM(address);
    } else if (address < 0xFE00) {  // Reserved - Echo RAM
        return 0;
    } else if (address < 0xFEA0) {  // Object Attribute Memory
        NO_IMPLEMENTATION("readBus() for Object Attribute Memory");
    } else if (address < 0xFF00) {  // Reserved - Unusable
        return 0;
    } else if (address < 0xFF80) {  // I/O Registers
        NO_IMPLEMENTATION("readBus() for I/O Registers");
    } else if (address == 0xFFFF) {  // CPU Interrupt Enable Register
        return readCPUIERegister();
    } else {  // High RAM
        return readHighRAM(address);
    }
}

/**
 * Reads 16 bits from the bus at the given address.
 *
 * @param address The address to read from.
 * @return The 16 bits read from the bus.
 */
u16 readBus16(u16 address) {
    u16 lo = readBus(address);
    u16 hi = readBus(address + 1);

    return lo | (hi << 8);
}

/**
 * Writes a byte to the bus at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeBus(u16 address, u8 value) {
    if (address < 0x8000) {  // ROM data
        return writeToCartridge(address, value);
    } else if (address < 0xA000) {  // Character/Map data
        NO_IMPLEMENTATION("writeBus() for Character/Map data");
    } else if (address < 0xC000) {  // Cartridge RAM
        return writeToCartridge(address, value);
    } else if (address < 0xE000) {  // Working RAM
        return writeToWorkingRAM(address, value);
    } else if (address < 0xFE00) {  // Reserved - Echo RAM
        return;
    } else if (address < 0xFEA0) {  // Object Attribute Memory
        NO_IMPLEMENTATION("writeBus() for Object Attribute Memory");
    } else if (address < 0xFF00) {  // Reserved - Unusable
        return;
    } else if (address < 0xFF80) {  // I/O Registers
        NO_IMPLEMENTATION("writeBus() for I/O Registers");
    } else if (address == 0xFFFF) {  // CPU Interrupt Enable Register
        return setCPUIERegister(value);
    } else {
        return writeToHighRAM(address, value);
    }
}

/**
 * Writes 16 bits to the bus at the given address.
 *
 * @param address The address to write to.
 * @param value The 16 bits to write.
 */
void writeBus16(u16 address, u16 value) {
    writeBus(address + 1, (value >> 8) & 0xFF);
    writeBus(address, value & 0xFF);
}