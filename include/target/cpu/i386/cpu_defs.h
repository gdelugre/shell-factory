#ifndef CPU_REGS_H_
#define CPU_REGS_H_

#define SP_REGISTER "esp"

namespace CPU {

    using reg_t     = uint32_t;
    using addr_t    = uint32_t;

    enum registers
    {
        // General registers.
        CPU_REG_EAX,
        CPU_REG_EBX,
        CPU_REG_ECX,
        CPU_REG_EDX,
        CPU_REG_ESI,
        CPU_REG_EDI,
        CPU_REG_EBP,
        CPU_REG_ESP,
        CPU_REG_EIP,

        // Flags.
        CPU_REG_EFLAGS,

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
}

#endif
