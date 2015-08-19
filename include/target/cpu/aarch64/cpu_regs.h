#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "sp"

enum cpu_registers
{
    REG_X0,
    REG_X1,
    REG_X2,
    REG_X3,
    REG_X4,
    REG_X5,
    REG_X6,
    REG_X7,
    REG_X8,
    REG_X9,
    REG_X10,
    REG_X11,
    REG_X12,
    REG_X13,
    REG_X14,
    REG_X15,
    REG_X16,
    REG_X17,
    REG_X18,
    REG_X19,
    REG_X20,
    REG_X21,
    REG_X22,
    REG_X23,
    REG_X24,
    REG_X25,
    REG_X26,
    REG_X27,
    REG_X28,
    REG_X29,
    REG_X30,
    REG_X31,
    REG_SP,
    REG_PC,
    REG_PSTATE,
};

typedef uint64_t cpu_reg_t;

#endif
