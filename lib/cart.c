// * Contains cartridge functions for loading and reading from the ROM.

#include <cart.h>

// ===== Globals ===============================================================

// Keeps track of the cartridge state
static cartContext_t ctx;

// Map lookup for various cartridge types
static const char *CARTRIDGE_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM",          // No licensed cartridge uses this option
    "ROM+RAM+BATTERY",  // No licensed cartridge uses this option
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY",  // MBC3 with 64KiB SRAM is Pokemon Crystal
    "MBC3",
    "MBC3+RAM",  // MBC3 with 64KiB SRAM is Pokemon Crystal
    "MBC3+RAM+BATTERY",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
    // Excluding $FC POCKET CAMERA
    // Excluding $FD BANDAI TAMA5
    // Excluding $FE HuC3
    // Excluding $FF HuC1+RAM+BATTERY
};

// Licensed manufacturer codes map lookup
static const char *LIC_CODES[0xA5] = {[0x00] = "None",
                                      [0x01] = "Nintendo R&D1",
                                      [0x08] = "Capcom",
                                      [0x13] = "Electronic Arts",
                                      [0x18] = "Hudson Soft",
                                      [0x19] = "b-ai",
                                      [0x20] = "kss",
                                      [0x22] = "pow",
                                      [0x24] = "PCM Complete",
                                      [0x25] = "san-x",
                                      [0x28] = "Kemco Japan",
                                      [0x29] = "seta",
                                      [0x30] = "Viacom",
                                      [0x31] = "Nintendo",
                                      [0x32] = "Bandai",
                                      [0x33] = "Ocean/Acclaim",
                                      [0x34] = "Konami",
                                      [0x35] = "Hector",
                                      [0x37] = "Taito",
                                      [0x38] = "Hudson",
                                      [0x39] = "Banpresto",
                                      [0x41] = "Ubi Soft",
                                      [0x42] = "Atlus",
                                      [0x44] = "Malibu",
                                      [0x46] = "angel",
                                      [0x47] = "Bullet-Proof",
                                      [0x49] = "irem",
                                      [0x50] = "Absolute",
                                      [0x51] = "Acclaim",
                                      [0x52] = "Activision",
                                      [0x53] = "American sammy",
                                      [0x54] = "Konami",
                                      [0x55] = "Hi tech entertainment",
                                      [0x56] = "LJN",
                                      [0x57] = "Matchbox",
                                      [0x58] = "Mattel",
                                      [0x59] = "Milton Bradley",
                                      [0x60] = "Titus",
                                      [0x61] = "Virgin",
                                      [0x64] = "LucasArts",
                                      [0x67] = "Ocean",
                                      [0x69] = "Electronic Arts",
                                      [0x70] = "Infogrames",
                                      [0x71] = "Interplay",
                                      [0x72] = "Broderbund",
                                      [0x73] = "sculptured",
                                      [0x75] = "sci",
                                      [0x78] = "THQ",
                                      [0x79] = "Accolade",
                                      [0x80] = "misawa",
                                      [0x83] = "lozc",
                                      [0x86] = "Tokuma Shoten Intermedia",
                                      [0x87] = "Tsukuda Original",
                                      [0x91] = "Chunsoft",
                                      [0x92] = "Video system",
                                      [0x93] = "Ocean/Acclaim",
                                      [0x95] = "Varie",
                                      [0x96] = "Yonezawa/s'pal",
                                      [0x97] = "Kaneko",
                                      [0x99] = "Pack in soft",
                                      [0xA4] = "Konami (Yu-Gi-Oh!)"};

// ===== Helper functions ======================================================

/**
 * Gets the name of the cartridge's licensee code.
 *
 * @return The name of the licensee code.
 */
const char *getLicenseeName() {
    if (ctx.header->newLICCode <= 0xA4) {
        return LIC_CODES[ctx.header->oldLICCode];
    }

    return "UNKNOWN";
}

/**
 * Gets the name of the cartridge's type.
 *
 * @return The name of the cartridge type.
 */
const char *getCartridgeType() {
    if (ctx.header->type <= 0x22) {
        return CARTRIDGE_TYPES[ctx.header->type];
    }

    return "UNKNOWN";
}

// ===== Cartridge functions ===================================================

/**
 * Loads a cartridge into the emulator based on filename.
 * This function will load the ROM data into memory and verify the checksum.
 *
 * @param filename The filename of the cartridge to load.
 * @return Whether the cartridge was loaded successfully.
 */
bool loadCartridge(char *filename) {
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return false;
    }

    // Determine maximal cartridge size
    fseek(fp, 0, SEEK_END);
    ctx.ROMSize = ftell(fp);

    // Now reset back to the beginning
    rewind(fp);

    // Read in all ROM data
    ctx.ROMData = malloc(ctx.ROMSize);
    fread(ctx.ROMData, ctx.ROMSize, 1, fp);
    fclose(fp);

    ctx.header = (ROMHeader_t *)(ctx.ROMData + 0x100);
    ctx.header->title[15] = 0;  // Ensure title names are null-terminated

    printf("Cartridge Loaded from file %s%s%s:\n", CCYN, ctx.filename, CRST);
    printf("\tTitle    : %s%s%s\n", CBLU, ctx.header->title, CRST);
    printf("\tType     : %s0x%2.2X%s (%s)\n", CMAG, ctx.header->type, CRST,
           getCartridgeType());
    printf("\tROM Size : %s0x%2.2X%s (%s%d%s KiB)\n", CMAG, ctx.header->ROMSize,
           CRST, CYEL, 32 << ctx.header->ROMSize, CRST);

    // Calculate and return RAM sizing
    printf("\tRAM Size : %s0x%2.2X%s ", CMAG, ctx.header->RAMSize, CRST);
    switch (ctx.header->RAMSize) {
        case 0x00:
            printf("(No RAM)\n");
            break;
        case 0x01:
            printf("(Unused, %s2%s KiB)\n", CYEL, CRST);
            break;
        case 0x02:
            printf("(%s1%sx%s8%s KiB)\n", CYEL, CRST, CYEL, CRST);
            break;
        case 0x03:
            printf("(%s4%sx%s8%s KiB = %s32%s KiB)\n", CYEL, CRST, CYEL, CRST,
                   CYEL, CRST);
            break;
        case 0x04:
            printf("(%s16%sx%s8%s KiB = %s128%s KiB)\n", CYEL, CRST, CYEL, CRST,
                   CYEL, CRST);
            break;
        case 0x05:
            printf("(%s8%sx%s8%s KiB = %s64%s KiB)\n", CYEL, CRST, CYEL, CRST,
                   CYEL, CRST);
            break;
        default:
            printf("(%sUnknown RAM flag%s)\n", CRED, CRST);
    }

    printf("\tLIC Code : %s0x%2.2X%s (%s)\n", CMAG, ctx.header->oldLICCode,
           CRST, getLicenseeName());
    printf("\tROM Vers : %s0x%2.2X%s\n", CMAG, ctx.header->version, CRST);

    // Perform checksum algorithm
    u16 x = 0;
    for (u16 i = 0x0134; i <= 0x014C; i++) {
        x = x - ctx.ROMData[i] - 1;
    }
    // Verify checksum of ROM
    printf("\tChecksum : %s0x%2.2X%s (", CMAG, ctx.header->checksum, CRST);
    if (x & 0xFF)
        printf("%sPASSED%s)\n", CGRN, CRST);
    else
        printf("%sFAILED%s)\n", CRED, CRST);

    return true;
}

/**
 * Reads a byte from the cartridge at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the cartridge.
 */
u8 readCartridge(u16 address) {
    // For now, ROM ONLY supported
    return ctx.ROMData[address];
}

/**
 * Writes a byte to the cartridge at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToCartridge(u16 address, u8 value) {
    // For now, ROM ONLY supported
    NO_IMPL
}