#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <asm/unistd.h>
#include <stddef.h>

#define SHELLCODE_ENTRY _Noreturn void _start(void) { do {
#define SHELLCODE_END } while(0); for(;;); }

#define NO_RETURN __attribute__((noreturn))
#define GLOBAL_DECL static __attribute__((nocommon, section(".rodata")))
#define FUNCTION static inline __attribute__((section(".funcs")))
#define SYSTEM_CALL static inline

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#if defined(__i386__)
#include <sysdeps/unix/sysv/linux/i386/sysdep.h>
#elif defined(__x86_64__)
#include <sysdeps/unix/sysv/linux/x86_64/sysdep.h>
#elif defined(__arm__)
#include <sysdeps/unix/sysv/linux/arm/sysdep.h>
#else
#error "Architecture not implemented."
#endif

#endif /* _SYSTEM_H */
