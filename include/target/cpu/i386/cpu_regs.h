#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "esp"

enum cpu_registers
{
    // General registers.
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_ESI,
    REG_EDI,
    REG_EBP,
    REG_ESP,
    REG_EIP,

    // Flags.
    REG_EFLAGS,

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

typedef uint32_t cpu_reg_t;

#endif
