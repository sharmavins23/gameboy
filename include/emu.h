#pragma once

#include <common.h>

// Emulator context object, which keeps track of the emulator's state
typedef struct {
    bool paused;   // Whether the emulator is paused
    bool running;  // Whether the emulator is running
    bool die;      // Whether the emulator should exit
    u64 ticks;     // Processor ticks
} emuContext_t;

/**
 * Gets the emulator's context object.
 *
 * @return The emulator's context object.
 */
emuContext_t *getEMUContext();

/**
 * Runs the emulator system with the given arguments.
 * Acts as a secondary entry point to the emulator.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The exit code.
 */
int runEmulator(int argc, char **argv);

/**
 * Emulates a given number of CPU cycles.
 * This function is used to emulate elapsed time caused by CPU instructions.
 *
 * @param cpuCycles The number of CPU cycles to emulate.
 */
void emulateCPUCycles(int cpuCycles);