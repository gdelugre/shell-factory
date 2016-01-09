#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "rsp"

enum cpu_registers
{
    // General registers.
    CPU_REG_RAX,
    CPU_REG_RBX,
    CPU_REG_RCX,
    CPU_REG_RDX,
    CPU_REG_RSI,
    CPU_REG_RDI,
    CPU_REG_RBP,
    CPU_REG_RSP,
    CPU_REG_R8,
    CPU_REG_R9,
    CPU_REG_R10,
    CPU_REG_R11,
    CPU_REG_R12,
    CPU_REG_R13,
    CPU_REG_R14,
    CPU_REG_R15,
    CPU_REG_RIP,

    // Flags.
    CPU_REG_RFLAGS,

    // Segment selectors.
    CPU_REG_CS,
    CPU_REG_DS,
    CPU_REG_ES,
    CPU_REG_FS,
    CPU_REG_GS,
    CPU_REG_SS,

    // Debug registers.
    CPU_REG_DR0,
    CPU_REG_DR1,
    CPU_REG_DR2,
    CPU_REG_DR3,
    CPU_REG_DR4,
    CPU_REG_DR5,
    CPU_REG_DR6,
    CPU_REG_DR7,
};

typedef uint64_t cpu_reg_t;

#endif
