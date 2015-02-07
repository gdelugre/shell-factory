#ifndef _SYSCALL_ABI_H
#define _SYSCALL_ABI_H

/*
 * Calling convention for SysV amd64:
 *
 * syscall number: rax
 * arg1          : rdi
 * arg2          : rsi
 * arg3          : rdx
 * arg4          : rcx
 * arg5          : r8
 * arg6          : r9
 *
 * syscall
 *
 * return value to rax
 */

#define SYSCALL_CLOBBERED_REGISTERS "rcx", "r11"
#define SYSCALL_NUMBER_REGISTER "rax"
#define SYSCALL_RESULT_REGISTER "rax"
#define SYSCALL_INSTRUCTION     "syscall"
#define SYSCALL_PUSH_ARGUMENTS(...) \
    SYSCALL_ARG_BIND_REGISTER(0, "rdi", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, "rsi", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, "rdx", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, "rcx", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(4, "r8", __VA_ARGS__);  \
    SYSCALL_ARG_BIND_REGISTER(5, "r9", __VA_ARGS__);

#endif
