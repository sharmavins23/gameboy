#pragma once

#include <common.h>

// Cartridge structure for ease of use
typedef struct {
    u8 entry[4];     // Entrypoint for boot ROM
    u8 logo[0x30];   // Nintendo logo - Should match a hexadecimal dump
    char title[16];  // ASCII string of the game title
    // NOTE: Title contains manufacturer code and CGB flag

    u16 newLICCode;       // Licensee code for game publisher
    u8 SGBFlag;           // Whether the game supports running on SNES consoles
    u8 type;              // Cartridge types - See CARTRIDGE_TYPES array
    u8 ROMSize;           // Length of ROM - Usually 32 KiB * (1 << value)
    u8 RAMSize;           // How much RAM is present on the cartridge, if any
    u8 destinationCode;   // Whether the game is sold in Japan or not
    u8 oldLICCode;        // Specifies game publisher, or 0x33 if newLICCode
    u8 version;           // Version number for the game
    u8 checksum;          // Header checksum
    u16 global_checksum;  // Sum of all bytes in ROM, excluding last two
} ROMHeader_t;

// Cartridge context
typedef struct {
    char filename[1024];  // File name of the cartridge (ROM file)
    u32 ROMSize;          // ROM Sizing (in bytes)
    u8 *ROMData;          // Maximal size: 2MB
    ROMHeader_t *header;  // Header information
} cartContext_t;

/**
 * Loads a cartridge into the emulator based on filename.
 * This function will load the ROM data into memory and verify the checksum.
 *
 * @param filename The filename of the cartridge to load.
 * @return Whether the cartridge was loaded successfully.
 */
bool loadCartridge(char *filename);

/**
 * Reads a byte from the cartridge at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the cartridge.
 */
u8 readCartridge(u16 address);

/**
 * Writes a byte to the cartridge at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToCartridge(u16 address, u8 value);