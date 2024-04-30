#pragma once

#include <common.h>

// Emulator context object, which keeps track of the emulator's state
typedef struct {
    bool paused;   // Whether the emulator is paused
    bool running;  // Whether the emulator is running
    u64 ticks;     // Processor ticks
} emuContext_t;

/**
 * Gets the emulator's context object.
 *
 * @return The emulator's context object.
 */
emuContext_t *getEMUContext();

int emu_run(int argc, char **argv);

void emu_cycles(int cpu_cycles);