// * Implementation of main emulator components.

#include <stdio.h>
#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <common.h>

/**
 * The emulator has the following major components:
 * * Cart: The game cartridge, which is stored as a file.
 * * CPU: The emulated cycle-accurate CPU.
 * * Address Bus: A central place for read/write functionality.
 * * PPU: Pixel Processing Unit, which generates a video signal.
 * * Timer: Keeps track of time.
 */

// ===== Globals ===============================================================

// Keeps track of the emulator state
static emuContext_t ctx;

// ===== Helper functions ======================================================

/**
 * Gets the emulator's context object.
 *
 * @return The emulator's context object.
 */
emuContext_t *getEMUContext() { return &ctx; }

/**
 * Delays the processor for a given number of milliseconds.
 *
 * @param ms The number of milliseconds to delay.
 */
void delay(u32 ms) { SDL_Delay(ms); }

// ===== Emulator functions ====================================================

/**
 * Runs the emulator system with the given arguments.
 * Acts as a secondary entry point to the emulator.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The exit code.
 */
int emu_run(int argc, char **argv) {
    printf("%s=======================%s\n", CBLU, CRST);
    printf("%s * Game Boy Emulator * %s\n", CMAG, CRST);
    printf("%s=======================%s\n", CBLU, CRST);

    // Return if the user didn't provide a ROM file
    if (argc < 2) {
        printf("%sERR:%s No ROM file provided!\n", CRED, CRST);
        printf("Usage: %semu <rom_file>%s\n", CMAG, CRST);
        return EXIT_FAILURE;
    }
    // Try loading the cartridge
    if (!loadCartridge(argv[1])) {
        printf("%sERR:%s Failed to load ROM file: %s%s%s\n", CRED, CRST, CCYN,
               argv[1], CRST);
        return EXIT_FAILURE;
    }

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

    return EXIT_SUCCESS;
}

void emu_cycles(int cpu_cycles) {
    // TODO: Emulate cycle stalling
}