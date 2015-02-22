#ifndef FACTORY_H_
#define FACTORY_H_

#include <stddef.h>

#define NO_RETURN [[noreturn]]
#define GLOBAL_DECL static __attribute__((nocommon, section(".rodata")))
#define METHOD inline __attribute__((section(".funcs")))
#define FUNCTION static METHOD
#define SYSTEM_CALL static inline __attribute__((section(".funcs")))
#define BUILTIN(func) __builtin_ ## func

#ifndef NO_BUILTIN
#define NO_BUILTIN 0
#endif

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SHELLCODE_ENTRY NO_RETURN static void Shellcode::entry()

namespace Shellcode {
    NO_RETURN inline static void entry();
}

extern "C" {
    NO_RETURN void _start(void)
    {
        Shellcode::entry();
        for (;;);
    }
}

// XXX: Move to CPU definition header.
#define PAGE_SIZE 4096

#include <target/syscalls.h>

#endif
