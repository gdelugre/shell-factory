#ifndef FACTORY_H_
#define FACTORY_H_

// Unneeded and may conflict with our function definitions.
#undef _FORTIFY_SOURCE

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
#define EXPORT_ABI_FUNCTION __attribute__((visibility("internal")))
#define SYSTEM_CALL static INLINE
#define SYSTEM_CALL_INLINE static __attribute__((always_inline))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define PURE __attribute__((pure))
#define BUILTIN(func) __builtin_ ## func

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SHELLCODE_ENTRY NO_RETURN static void Shellcode::entry()

namespace Shellcode {
    NO_RETURN FUNCTION void entry();
}

extern "C" {

    // Shellcode entry point.
    NO_RETURN void _start(void)
    {
        Shellcode::entry();
        __builtin_unreachable();
    }

    // Exit destructors are not handled.
    EXPORT_ABI_FUNCTION
    int __cxa_atexit(void UNUSED (*f)(void *), UNUSED void *objptr, UNUSED void *dso) {
        return 0;
    }

    NO_RETURN
    EXPORT_ABI_FUNCTION
    void abort()
    {
        __builtin_trap();
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
        abort();
}
#endif

extern char __executable_start;
extern char etext;
extern char edata;
extern char ebss;
extern char eend;

#include <memops.h>
#include <softfloat.h>
#include <factory-options.h>
#include <target/cpu.h>
#include <target/memory.h>
#include <target/syscalls.h>

#endif
