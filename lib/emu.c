#include <stdio.h>
#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
  Emu components:

  |Cart|        - Cartridge
  |CPU|         - Emulated cycle-accurate CPU
  |Address Bus| - Central place for reading/writing
  |PPU|         - Pixel Processing Unit
  |Timer|       - Timer for keeping track of time
*/

static emu_context ctx;

emu_context *emu_get_context() { return &ctx; }

void delay(u32 ms) { SDL_Delay(ms); }

int emu_run(int argc, char **argv) {
    // Return if the user didn't provide a ROM file
    if (argc < 2) {
        printf("Usage: emu <rom_file>\n");
        return -1;
    }

    // Try loading the cartridge
    if (!cart_load(argv[1])) {
        printf("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    printf("Cart loaded..\n");

    // Initialize Simple DirectMedia Layer for rendering
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    // Initialize the TrueType Font library
    TTF_Init();
    printf("TTF INIT\n");

    cpu_init();

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    // Game loop
    while (ctx.running) {
        // Hang processor for paused game
        if (ctx.paused) {
            delay(10);
            continue;
        }

        // If the CPU hangs/fails stepping, stop the emulator
        if (!cpu_step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        ctx.ticks++;
    }

    return 0;
}
