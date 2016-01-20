#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#include <cstdint>

#define SP_REGISTER "r15"

namespace CPU {

    using reg_t     = uint32_t;
    using addr_t    = uint32_t;
    using word_t    = uint32_t;

    enum registers
    {
        // General registers.
        CPU_REG_R0,
        CPU_REG_R1,
        CPU_REG_R2,
        CPU_REG_R3,
        CPU_REG_R4,
        CPU_REG_R5,
        CPU_REG_R6,
        CPU_REG_R7,
        CPU_REG_R8,
        CPU_REG_R9,
        CPU_REG_R10,
        CPU_REG_R11,
        CPU_REG_R12,
        CPU_REG_R13,
        CPU_REG_R14,
        CPU_REG_R15,

        // FPU registers.
        CPU_REG_FP0,
        CPU_REG_FP1,
        CPU_REG_FP2,
        CPU_REG_FP3,
        CPU_REG_FP4,
        CPU_REG_FP5,
        CPU_REG_FP6,
        CPU_REG_FP7,
        CPU_REG_FP8,
        CPU_REG_FP9,
        CPU_REG_FP10,
        CPU_REG_FP11,
        CPU_REG_FP12,
        CPU_REG_FP13,
        CPU_REG_FP14,
        CPU_REG_FP15,

        // System registers.
        CPU_REG_PC,
        CPU_REG_PR,
        CPU_REG_FPSCR,
        CPU_REG_FPUL,
        CPU_REG_MACH,
        CPU_REG_MACL,

        // Control registers.
        CPU_REG_SR,
        CPU_REG_GBR,
    };
}

#endif
