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
#define CBLU "\e[0;34m"  // For titles
#define CMAG "\e[0;35m"  // For code/usage/help, hexadecimal
#define CCYN "\e[0;36m"  // For filenames
#define CRST "\e[0m"     // Reset after colors

#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#define BIT_SET(a, n, on)     \
    {                         \
        if (on)               \
            a |= (1 << n);    \
        else                  \
            (a) &= ~(1 << n); \
    }

#define BETWEEN(a, b, c) ((a >= b) && (a <= c))

void delay(u32 ms);

#define NO_IMPL                                   \
    {                                             \
        fprintf(stderr, "NOT YET IMPLEMENTED\n"); \
        exit(-5);                                 \
    }
