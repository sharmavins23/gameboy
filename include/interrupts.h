#pragma once

#include <cpu.h>

typedef enum {
    INT_VBLANK = 1,
    INT_LCDSTAT = 2,
    INT_TIMER = 4,
    INT_SERIAL = 8,
    INT_JOYPAD = 16
} interruptType_t;

/**
 * Handles interrupts for the CPU.
 *
 * @param ctx The CPU context.
 */
void handleCPUInterrupt(cpuContext_t *ctx);