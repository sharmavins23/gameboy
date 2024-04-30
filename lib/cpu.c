#include <cpu.h>
#include <bus.h>
#include <emu.h>

cpu_context ctx = {0};

void cpu_init() {
    // Set the program counter to the entrypoint
    ctx.regs.pc = 0x100;
    // Default value for A register
    ctx.regs.a = 0x01;
}

static void fetch_instruction() {
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
}

static void fetch_data() {
    ctx.mem_dest = 0;
    ctx.dest_is_mem = false;

    if (ctx.cur_inst == NULL) {
        return;  // Avoid segfaults
    }

    switch (ctx.cur_inst->mode) {
        case AM_IMP:  // Implied mode

            return;

        case AM_R:  // Register
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_1);

            return;

        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);  // 1 CPU cycle for bus reading
            ctx.regs.pc++;

            return;

        case AM_D16: {
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);
            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo | (hi << 8);
            ctx.regs.pc += 2;

            return;
        }

        default:
            printf("Unknown addressing mode! %d (%02X)\n", ctx.cur_inst->mode,
                   ctx.cur_opcode);
            exit(-7);  // Fatal error
    }
}

static void execute() {
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

    if (!proc) {
        NO_IMPL  // If null, no implementation
    }

    proc(&ctx);
}

bool cpu_step() {
    // If the CPU is running, fetch an instruction
    if (!ctx.halted) {
        u16 pc = ctx.regs.pc;

        fetch_instruction();
        fetch_data();

        printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n", pc,
               inst_name(ctx.cur_inst->type), ctx.cur_opcode, bus_read(pc + 1),
               bus_read(pc + 2), ctx.regs.a, ctx.regs.b, ctx.regs.c);

        if (ctx.cur_inst == NULL) {
            printf("Unknown instruction! %02X\n", ctx.cur_opcode);
            exit(-7);
        }

        execute();
    }

    return true;
}
