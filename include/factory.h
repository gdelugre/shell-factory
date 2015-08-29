#ifndef FACTORY_H_
#define FACTORY_H_

#include <stdint.h>
#include <stddef.h>

#if defined(RELAX_INLINE) && (RELAX_INLINE == 1)
#define INLINE inline
#else
#define INLINE inline __attribute__((always_inline))
#endif

#define NO_RETURN [[noreturn]]
#define GLOBAL_DECL static __attribute__((nocommon, section(".rodata")))
#define CONSTRUCTOR INLINE
#define DESTRUCTOR INLINE
#define METHOD_NOINLINE __attribute__((noinline))
#define METHOD INLINE
#define VIRTUAL_METHOD virtual METHOD
#define FUNCTION static METHOD
#define FUNCTION_NOINLINE static METHOD_NOINLINE
#define SYSTEM_CALL static INLINE
#define BUILTIN(func) __builtin_ ## func

#ifndef NO_BUILTIN
#define NO_BUILTIN 0
#endif

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SHELLCODE_ENTRY NO_RETURN static void Shellcode::entry()

namespace Shellcode {
    NO_RETURN INLINE static void entry();
}

extern "C" {
    NO_RETURN void _start(void)
    {
        Shellcode::entry();
        __builtin_unreachable();
    }
}

FUNCTION
void assert(bool expr)
{
    if ( !expr )
        __builtin_trap();
}

// TODO: Move to CPU definition header.
#define PAGE_SIZE 4096

#include <factory-options.h>
#include <target/cpu.h>
#include <target/syscalls.h>

#endif
