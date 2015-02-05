#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <asm/unistd.h>
#include <stddef.h>

#define NO_RETURN __attribute__((noreturn))
#define GLOBAL_DECL static __attribute__((nocommon, section(".rodata")))
#define FUNCTION static inline __attribute__((section(".funcs")))
#define SYSTEM_CALL static inline __attribute__((section(".funcs")))
#define BUILTIN(func) __builtin_ ## func

#ifndef NO_BUILTIN
#define NO_BUILTIN 0
#endif

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SHELLCODE_ENTRY [[noreturn]] void _start(void) { do {
#define SHELLCODE_END } while(0); for(;;); }

#if defined(__i386__)
#include <sysdeps/unix/sysv/linux/i386/sysdep.h>
#elif defined(__x86_64__)
#include <sysdeps/unix/sysv/linux/x86_64/sysdep.h>
#elif defined(__arm__)
#include <sysdeps/unix/sysv/linux/arm/sysdep.h>
#elif defined(__mips__) && (_MIPS_SZPTR == 32)
#include <sysdeps/unix/sysv/linux/mips/mips32/sysdep.h>
#else
#error "Architecture not implemented."
#endif

#if defined(__arm__) && defined(__thumb__)
__attribute__((section(".funcs"), naked))
void __libc_do_syscall(void)
{
    asm volatile (
        "push {r7, lr}\n"
        "mov r7, ip\n"
        "swi 0\n"
        "pop {r7, pc}\n"
    );
}
#endif

#define SYSCALL_EXISTS(name) defined(__NR_## name)

#define DO_SYSCALL(name, num_args, ...) ({ \
  int err_val; \
  (void) err_val; \
  INTERNAL_SYSCALL(name, err_val, num_args, __VA_ARGS__); \
  });

#endif /* _SYSTEM_H */
