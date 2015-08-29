#ifndef GENERIC_CPU_REGS_H_
#define GENERIC_CPU_REGS_H_

#if defined(__i386__)
#include <target/cpu/i386/cpu_regs.h>
#elif defined(__amd64__)
#include <target/cpu/amd64/cpu_regs.h>
#elif defined(__arm__)
#include <target/cpu/arm/softfloat.h>
#include <target/cpu/arm/cpu_regs.h>
#elif defined(__aarch64__)
#include <target/cpu/aarch64/cpu_regs.h>
#elif defined(__mips__)
#include <target/cpu/mips/cpu_regs.h>
#else
#error "No registers defined for this CPU."
#endif

#define FLUSH_VARIABLE(var) \
    asm("" :: "r" (var));

register char *__reg_sp asm ( SP_REGISTER );

namespace Stack {

    FUNCTION
    void shift(long offset)
    {
        __reg_sp += offset;
        FLUSH_VARIABLE(__reg_sp); 
    }

    FUNCTION
    void *pointer()
    {
        return __reg_sp;
    }
}

#endif
