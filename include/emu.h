#pragma once

#include <common.h>

typedef struct {
    bool paused;   // Whether the emulator is paused
    bool running;  // Whether the emulator is running
    u64 ticks;     // Processor ticks
} emu_context;

int emu_run(int argc, char **argv);

emu_context *emu_get_context();

void emu_cycles(int cpu_cycles);