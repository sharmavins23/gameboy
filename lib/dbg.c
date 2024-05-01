// * Handles debugging functions, used for Blargg tests.

#include <dbg.h>
#include <bus.h>

// ===== Globals ===============================================================

// Holds the current debug message
static char debugMessage[1024] = {0};
static int messageSize = 0;

// ===== Debug functions =======================================================

/**
 * Updates the debug message with the latest serial data.
 */
void debugUpdate() {
    if (readBus(0xFF02) & 0x81) {
        char c = readBus(0xFF01);
        debugMessage[messageSize++] = c;

        writeBus(0xFF02, 0);
    }
}

/**
 * Prints the current debug message.
 */
void debugPrint() {
    if (debugMessage[0]) {
        printf("%sDebug:%s %s\n", CYEL, CRST, debugMessage);
    }
}