#pragma once

#include <common.h>

/**
 * Reads a byte from the bus at the given address.
 *
 * @param address The address to read from.
 * @return The byte read from the bus.
 */
u8 readFromBus(u16 address);

/**
 * Reads 16 bits from the bus at the given address.
 *
 * @param address The address to read from.
 * @return The 16 bits read from the bus.
 */
u16 read16FromBus(u16 address);

/**
 * Writes a byte to the bus at the given address.
 *
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeToBus(u16 address, u8 value);

/**
 * Writes 16 bits to the bus at the given address.
 *
 * @param address The address to write to.
 * @param value The 16 bits to write.
 */
void write16ToBus(u16 address, u16 value);