#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "rsp"

enum cpu_registers
{
    // General registers.
    REG_RAX,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RSI,
    REG_RDI,
    REG_RSI,
    REG_RBP,
    REG_RSP,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    REG_RIP,

    // Flags.
    REG_RFLAGS,

    // Segment selectors.
    REG_CS,
    REG_DS,
    REG_ES,
    REG_FS,
    REG_GS,
    REG_SS,

    // Debug registers.
    REG_DR0,
    REG_DR1,
    REG_DR2,
    REG_DR3,
    REG_DR4,
    REG_DR5,
    REG_DR6,
    REG_DR7,
};

typedef uint64_t cpu_reg_t;

#endif
