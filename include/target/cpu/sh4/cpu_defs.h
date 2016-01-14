#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "r15"

namespace CPU {

    using reg_t     = uint32_t;
    using addr_t    = uint32_t;
    using word_t    = uint32_t;

    enum registers
    {
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
    };
}

#endif
