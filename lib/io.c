// * Handles all I/O.

#include <io.h>
#include <common.h>

// ===== Globals ===============================================================

// Holds serial data during reading
static char serialData[2];

// ===== I/O functions =========================================================

/**
 * Reads a byte from the I/O registers at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the I/O registers.
 */
u8 readIO(u16 address) {
    if (address == 0xFF01) {
        return serialData[0];
    } else if (address == 0xFF02) {
        return serialData[1];
    }

    printf("%sERR:%s Unhandled I/O read at address 0x%04X\n", CRED, CRST,
           address);
    return 0;
}

/**
 * Writes a byte to the I/O registers at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeIO(u16 address, u8 value) {
    if (address == 0xFF01) {
        serialData[0] = value;
        return;
    }

    if (address == 0xFF02) {
        serialData[1] = value;
        return;
    }

    printf("%sERR:%s Unhandled I/O write at address 0x%04X\n", CRED, CRST,
           address);
}