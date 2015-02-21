#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for SysV x86:
 *
 * syscall number: eax
 * arg1          : ebx
 * arg2          : ecx
 * arg3          : edx
 * arg4          : esi
 * arg5          : edi
 * arg6          : ebp
 *
 * syscall
 *
 * return value to eax
 */

#define SYSCALL_CLOBBERED_REGISTERS "ecx", "edx" 
#define SYSCALL_NUMBER_REGISTER "eax"
#define SYSCALL_RESULT_REGISTER "eax"
#define SYSCALL_INSTRUCTION     "int $0x80"
#define SYSCALL_PUSH_ARGUMENTS(...) \
    SYSCALL_ARG_BIND_REGISTER(0, "ebx", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, "ecx", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, "edx", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, "esi", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(4, "edi", __VA_ARGS__);  \
    SYSCALL_ARG_BIND_REGISTER(5, "ebp", __VA_ARGS__);

#endif
