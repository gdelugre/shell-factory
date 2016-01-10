#ifndef FACTORY_H_
#define FACTORY_H_

#include <cstdint>
#include <cstddef>

#if defined(RELAX_INLINE) && (RELAX_INLINE == 1)
#define INLINE inline
#elif defined(RELAX_INLINE) && (RELAX_INLINE == 2)
#define INLINE
#elif defined(RELAX_INLINE) && (RELAX_INLINE == 3)
#define INLINE __attribute__((noinline))
#else
#define INLINE inline __attribute__((always_inline))
#endif

#define NO_RETURN [[noreturn]]
#define CONSTRUCTOR INLINE
#define DESTRUCTOR INLINE
#define METHOD_NOINLINE __attribute__((noinline))
#define METHOD INLINE
#define VIRTUAL_METHOD virtual METHOD
#define FUNCTION static METHOD
#define FUNCTION_NOINLINE static METHOD_NOINLINE
#define EXPORT_FUNCTION METHOD_NOINLINE
#define SYSTEM_CALL static INLINE
#define BUILTIN(func) __builtin_ ## func

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SHELLCODE_ENTRY NO_RETURN static void Shellcode::entry()

namespace Shellcode {
    NO_RETURN FUNCTION void entry();
}

extern "C" {
    NO_RETURN void _start(void)
    {
        Shellcode::entry();
        __builtin_unreachable();
    }
}

#if defined(NO_ASSERTS) && (NO_ASSERTS == 1)
#define assert(expr)
#else
FUNCTION
void assert(bool expr)
{
    if ( !expr )
        __builtin_trap();
}
#endif

// TODO: Move to CPU definition header.
#define PAGE_SIZE 4096

#include <softfloat.h>
#include <factory-options.h>
#include <target/cpu.h>
#include <target/syscalls.h>

#endif
