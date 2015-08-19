#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "sp"

enum cpu_registers
{
    // General registers.
    CPU_REG_ZERO,
    CPU_REG_AT,
    CPU_REG_V0,
    CPU_REG_V1,
    CPU_REG_A0,
    CPU_REG_A1,
    CPU_REG_A2,
    CPU_REG_A3,
    CPU_REG_T0
    CPU_REG_T1,
    CPU_REG_T2,
    CPU_REG_T3,
    CPU_REG_T4,
    CPU_REG_T5,
    CPU_REG_T6,
    CPU_REG_T7,
    CPU_REG_S0,
    CPU_REG_S1,
    CPU_REG_S2,
    CPU_REG_S3,
    CPU_REG_S4,
    CPU_REG_S5,
    CPU_REG_S6,
    CPU_REG_S7,
    CPU_REG_T8,
    CPU_REG_T9,

    // Unavailable registers.
    // CPU_REG_K0,
    // CPU_REG_K1,

    CPU_REG_GP,
    CPU_REG_SP,
    CPU_REG_S8,
    CPU_REG_RA,
    
    // Division registers.
    CPU_REG_LO,
    CPU_REG_HI,

    // Special registers.
    CPU_REG_EPC,
    CPU_REG_BADVADDR,
    CPU_REG_STATUS,
    CPU_REG_CAUSE,
};

typedef uint32_t cpu_reg_t;

#endif
