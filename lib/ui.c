// * Handles UI (and rendering).

#include <ui.h>
#include <emu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// ===== Globals ===============================================================

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *sdlSurface;

// ===== Helper functions ======================================================

/**
 * Delays the processor for a given number of milliseconds.
 *
 * @param ms The number of milliseconds to delay.
 */
void delay(u32 ms) { SDL_Delay(ms); }

// ===== UI functions ==========================================================

/**
 * Initializes SDL and the UI.
 */
void initializeUI() {
    // Initialize Simple DirectMedia Layer for rendering
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf(
            "%sERR:%s Failed to initialize Simple DirectMedia Layer (%sSDL%s): "
            "%s\n",
            CRED, CRST, CBLU, CRST, SDL_GetError());
        exit(EXIT_FAILURE);
    } else {
        printf("Initialized Simple DirectMedia Layer (%sSDL%s).\n", CBLU, CRST);
    }
    // Initialize the TrueType Font library
    if (TTF_Init() < 0) {
        printf(
            "%sERR:%s Failed to initialize TrueType Font library "
            "(%sSDL_ttf%s): "
            "%s\n",
            CRED, CRST, CBLU, CRST, TTF_GetError());
        exit(EXIT_FAILURE);
    } else {
        printf("Initialized TrueType Font library (%sSDL_ttf%s).\n", CBLU,
               CRST);
    }

    // Initialize window and renderer
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow,
                                &sdlRenderer);
}

/**
 * Handles UI events.
 */
void handleUIEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) > 0) {
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE) {
            getEMUContext()->die = true;
        }
    }
}