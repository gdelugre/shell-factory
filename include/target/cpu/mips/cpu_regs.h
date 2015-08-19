#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "sp"

enum cpu_registers
{
    // General registers.
    REG_ZERO,
    REG_AT,
    REG_V0,
    REG_V1,
    REG_A0,
    REG_A1,
    REG_A2,
    REG_A3,
    REG_T0
    REG_T1,
    REG_T2,
    REG_T3,
    REG_T4,
    REG_T5,
    REG_T6,
    REG_T7,
    REG_S0,
    REG_S1,
    REG_S2,
    REG_S3,
    REG_S4,
    REG_S5,
    REG_S6,
    REG_S7,
    REG_T8,
    REG_T9,

    // Unavailable registers.
    // REG_K0,
    // REG_K1,

    REG_GP,
    REG_SP,
    REG_S8,
    REG_RA,
    
    // Division registers.
    REG_LO,
    REG_HI,

    // Special registers.
    REG_EPC,
    REG_BADVADDR,
    REG_STATUS,
    REG_CAUSE,
};

typedef uint32_t cpu_reg_t;

#endif
