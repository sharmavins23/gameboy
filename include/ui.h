#pragma once

#include <common.h>

// Window dimensions
static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 768;

/**
 * Delays the processor for a given number of milliseconds.
 *
 * @param ms The number of milliseconds to delay.
 */
void delay(u32 ms);

/**
 * Initializes the UI.
 *
 * @return True if initialization was successful, false otherwise.
 */
void initializeUI();

/**
 * Handles UI events.
 */
void handleUIEvents();