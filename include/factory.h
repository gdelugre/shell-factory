#ifndef _SYSTEM_H
#define _SYSTEM_H

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

/*
#define SHELLCODE_ENTRY extern "C" {                            \
                            NO_RETURN void _start(void) {       \
                                do {

#define SHELLCODE_END           } while(0);                     \
                                for(;;);                        \
                            }                                   \
                        }
*/

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

#include <target/syscall_abi.h>

#define DO_SYSCALL(name, num_args, ...) ({ \
  int err_val; \
  (void) err_val; \
  EMIT_SYSCALL(name, __VA_ARGS__); \
  });

#endif /* _SYSTEM_H */
