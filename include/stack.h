#pragma once

#include <common.h>

/**
 * Pushes a byte onto the stack.
 *
 * @param data The data to push.
 */
void pushStack(u8 data);

/**
 * Pops a byte from the stack.
 *
 * @return The popped value.
 */
u8 popStack();

/**
 * Pushes a 16-bit value onto the stack.
 *
 * @param data The data to push.
 */
void pushStack16(u16 data);

/**
 * Pops a 16-bit value from the stack.
 *
 * @return The popped value.
 */
u16 popStack16();