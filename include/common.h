#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// * Type definitions
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// * Color codes
#define CRED "\e[0;31m"  // For errors
#define CGRN "\e[0;32m"  // For success
#define CYEL "\e[0;33m"  // For warnings, numbers
#define CBLU "\e[0;34m"  // For titles, readable names
#define CMAG "\e[0;35m"  // For code/usage/help, hexadecimal
#define CCYN "\e[0;36m"  // For filenames or highlights
#define CRST "\e[0m"     // Reset after colors

/**
 * Exits the program upon encountering a feature that has not yet been
 * implemented.
 *
 * @param feature The feature that has not yet been implemented.
 */
static inline void NO_IMPLEMENTATION(char* feature) {
    if (feature == NULL)
        printf("%sERR:%s Feature not yet implemented.\n", CRED, CRST);
    else
        printf("%sERR:%s Feature %s%s%s not yet implemented.\n", CRED, CRST,
               CBLU, feature, CRST);
    // ! DISABLE TO CONTINUE ON NI - NOT RECOMMENDED
    exit(EXIT_FAILURE);
}

/**
 * Retrieves a bit from a byte.
 *
 * @param a The byte to retrieve the bit from.
 * @param n The bit to retrieve.
 * @return The bit at the given position.
 */
static inline bool BIT(u8 a, u8 n) { return (a & (1 << n)) ? 1 : 0; }

/**
 * Sets a bit in a byte. This function RETURNS the new byte value.
 *
 * @param a The byte to set the bit in.
 * @param n The bit to set.
 * @param on Whether to set the bit on or off.
 */
static inline u8 SETBIT(u8 a, u8 n, bool on) {
    if (on)
        a |= (1 << n);
    else
        a &= ~(1 << n);

    return a;
}

/**
 * Checks if a value is between two other values.
 *
 * @param a The value to check.
 * @param b The lower bound.
 * @param c The upper bound.
 * @return Whether the value is between the two bounds.
 */
static inline bool BETWEEN(u8 a, u8 b, u8 c) { return (a >= b) && (a <= c); }

/**
 * Delays the processor for a given number of milliseconds.
 *
 * @param ms The number of milliseconds to delay.
 */
void delay(u32 ms);
