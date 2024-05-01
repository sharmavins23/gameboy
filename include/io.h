#pragma once

#include <common.h>

/**
 * Reads a byte from the I/O registers at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the I/O registers.
 */
u8 readIO(u16 address);

/**
 * Writes a byte to the I/O registers at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeIO(u16 address, u8 value);