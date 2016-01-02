#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for SysV amd64:
 *
 * syscall number: rax
 * arg1          : rdi
 * arg2          : rsi
 * arg3          : rdx
 * arg4          : r10
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

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "rdi", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "rsi", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "rdx", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "r10", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "r8", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "r9", value)
#define SYSCALL_SET_ARG_7(value) static_assert(false, "Too many arguments for this architecture.")

#endif
