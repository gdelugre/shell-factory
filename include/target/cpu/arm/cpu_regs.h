#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "sp"

enum cpu_registers
{
    REG_R0,
    REG_R1,
    REG_R2,
    REG_R3,
    REG_R4,
    REG_R5,
    REG_R6,
    REG_R7,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_FP,
    REG_IP,
    REG_SP,
    REG_LR,
    REG_PC,
    REG_CPSR,
};

typedef uint32_t cpu_reg_t;

#endif
