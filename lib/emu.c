// * Implementation of main emulator components.

#include <stdio.h>
#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <ui.h>
#include <pthread.h>
#include <unistd.h>

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
 * Separate thread to run the CPU.
 */
void *runCPU(void *ptr) {
    // Initialize CPU
    initializeCPU();

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    printf("Starting emulation...\n");

    // Run loop
    while (ctx.running) {
        // Hang processor for paused game
        if (ctx.paused) {
            delay(10);
            continue;
        }

        // Step the CPU
        stepCPU();

        ctx.ticks++;
    }

    return 0;
}

// ===== Emulator functions ====================================================

/**
 * Runs the emulator system with the given arguments.
 * Acts as a secondary entry point to the emulator.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The exit code.
 */
int runEmulator(int argc, char **argv) {
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

    // Initialize UI
    initializeUI();

    // Initialize CPU thread
    pthread_t cpuThread;
    if (pthread_create(&cpuThread, NULL, runCPU, NULL)) {
        printf("%sERR:%s Failed to create CPU thread.\n", CRED, CRST);
        return EXIT_FAILURE;
    }

    // Now simply poll the context to see if it's alive
    while (!ctx.die) {
        usleep(1000);  // Poll every 1ms
        handleUIEvents();
    }

    return EXIT_SUCCESS;
}

/**
 * Emulates a given number of CPU cycles.
 * This function is used to emulate elapsed time caused by CPU instructions.
 *
 * @param cpuCycles The number of CPU cycles to emulate.
 */
void emulateCPUCycles(int cpuCycles) {
    // TODO: Emulate cycle stalling
}