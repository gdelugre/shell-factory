#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "sp"

namespace CPU {

    using reg_t     = uint64_t;
    using addr_t    = uint64_t;
    using word_t    = uint64_t;

    enum registers
    {
        CPU_REG_X0,
        CPU_REG_X1,
        CPU_REG_X2,
        CPU_REG_X3,
        CPU_REG_X4,
        CPU_REG_X5,
        CPU_REG_X6,
        CPU_REG_X7,
        CPU_REG_X8,
        CPU_REG_X9,
        CPU_REG_X10,
        CPU_REG_X11,
        CPU_REG_X12,
        CPU_REG_X13,
        CPU_REG_X14,
        CPU_REG_X15,
        CPU_REG_X16,
        CPU_REG_X17,
        CPU_REG_X18,
        CPU_REG_X19,
        CPU_REG_X20,
        CPU_REG_X21,
        CPU_REG_X22,
        CPU_REG_X23,
        CPU_REG_X24,
        CPU_REG_X25,
        CPU_REG_X26,
        CPU_REG_X27,
        CPU_REG_X28,
        CPU_REG_X29,
        CPU_REG_X30,
        CPU_REG_X31,
        CPU_REG_SP,
        CPU_REG_PC,
        CPU_REG_PSTATE,
    };
}

#endif
