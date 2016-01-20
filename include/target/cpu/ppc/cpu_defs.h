#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#include <cstdint>

#define SP_REGISTER "r1"

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
        CPU_REG_R16,
        CPU_REG_R17,
        CPU_REG_R18,
        CPU_REG_R19,
        CPU_REG_R20,
        CPU_REG_R21,
        CPU_REG_R22,
        CPU_REG_R23,
        CPU_REG_R24,
        CPU_REG_R25,
        CPU_REG_R26,
        CPU_REG_R27,
        CPU_REG_R28,
        CPU_REG_R29,
        CPU_REG_R30,
        CPU_REG_R31,

        // FPU registers.
        CPU_REG_F0,
        CPU_REG_F1,
        CPU_REG_F2,
        CPU_REG_F3,
        CPU_REG_F4,
        CPU_REG_F5,
        CPU_REG_F6,
        CPU_REG_F7,
        CPU_REG_F8,
        CPU_REG_F9,
        CPU_REG_F10,
        CPU_REG_F11,
        CPU_REG_F12,
        CPU_REG_F13,
        CPU_REG_F14,
        CPU_REG_F15,
        CPU_REG_F16,
        CPU_REG_F17,
        CPU_REG_F18,
        CPU_REG_F19,
        CPU_REG_F20,
        CPU_REG_F21,
        CPU_REG_F22,
        CPU_REG_F23,
        CPU_REG_F24,
        CPU_REG_F25,
        CPU_REG_F26,
        CPU_REG_F27,
        CPU_REG_F28,
        CPU_REG_F29,
        CPU_REG_F30,
        CPU_REG_F31,

        // Special registers.
        CPU_REG_PC,
        CPU_REG_LR,
        CPU_REG_CTR,
        CPU_REG_XER,
        CPU_REG_FPSCR,
        CPU_REG_CR,
    };
}

#endif
