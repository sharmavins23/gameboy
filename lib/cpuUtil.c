// * Utility functions for the CPU.

#include <cpu.h>

// ===== Globals ===============================================================

// The CPU context object - contains all CPU state
extern cpuContext_t ctx;

// ===== Helper functions ======================================================

/**
 * Reverses the byte order of a 16-bit integer.
 *
 * @param n The 16-bit integer.
 * @return The reversed 16-bit integer.
 */
u16 reverse(u16 n) { return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8); }

/**
 * Reads a CPU register.
 *
 * @param registerType The register type.
 */
u16 readCPURegister(registerType_t registerType) {
    switch (registerType) {
        case RT_A:
            return ctx.registers.a;
        case RT_F:
            return ctx.registers.f;
        case RT_B:
            return ctx.registers.b;
        case RT_C:
            return ctx.registers.c;
        case RT_D:
            return ctx.registers.d;
        case RT_E:
            return ctx.registers.e;
        case RT_H:
            return ctx.registers.h;
        case RT_L:
            return ctx.registers.l;

        case RT_AF:
            return reverse(*((u16 *)&ctx.registers.a));
        case RT_BC:
            return reverse(*((u16 *)&ctx.registers.b));
        case RT_DE:
            return reverse(*((u16 *)&ctx.registers.d));
        case RT_HL:
            return reverse(*((u16 *)&ctx.registers.h));

        case RT_PC:
            return ctx.registers.pc;
        case RT_SP:
            return ctx.registers.sp;
        default:
            return 0;
    }
}

/**
 * Writes a value to a CPU register.
 *
 * @param registerType The register type.
 * @param value The value to write.
 */
void setCPURegister(registerType_t registerType, u16 value) {
    switch (registerType) {
        case RT_A:
            ctx.registers.a = value & 0xFF;
            return;
        case RT_F:
            ctx.registers.f = value & 0xFF;
            return;
        case RT_B:
            ctx.registers.b = value & 0xFF;
            return;
        case RT_C:
            ctx.registers.c = value & 0xFF;
            return;
        case RT_D:
            ctx.registers.d = value & 0xFF;
            return;
        case RT_E:
            ctx.registers.e = value & 0xFF;
            return;
        case RT_H:
            ctx.registers.h = value & 0xFF;
            return;
        case RT_L:
            ctx.registers.l = value & 0xFF;
            return;

        case RT_AF:
            *((u16 *)&ctx.registers.a) = reverse(value);
            return;
        case RT_BC:
            *((u16 *)&ctx.registers.b) = reverse(value);
            return;
        case RT_DE:
            *((u16 *)&ctx.registers.d) = reverse(value);
            return;
        case RT_HL:
            *((u16 *)&ctx.registers.h) = reverse(value);
            return;

        case RT_PC:
            ctx.registers.pc = value;
            return;
        case RT_SP:
            ctx.registers.sp = value;
            return;
        case RT_NONE:
            return;
    }
}

/**
 * Reads the CPU Interrupt Enable (IE) register.
 *
 * @return The value of the IE register.
 */
u8 readCPUIERegister() { return ctx.interruptEnableRegister; }

/**
 * Writes a value to the CPU Interrupt Enable (IE) register.
 *
 * @param value The value to write.
 */
void setCPUIERegister(u8 value) { ctx.interruptEnableRegister = value; }