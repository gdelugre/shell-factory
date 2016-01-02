#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

#define __ARCH_WANT_SYSCALL_NO_AT 1
#define __ARCH_WANT_SYSCALL_DEPRECATED 1

/*
 * Calling convention for aarch64 ABI: 
 *
 * syscall number: x8
 * arg1          : x0
 * arg2          : x1
 * arg3          : x2
 * arg4          : x3
 * arg5          : x4
 * arg6          : x5
 *
 * svc 0
 *
 * return value to x0
 */

#define SYSCALL_CLOBBERED_REGISTERS "x1", "x2", "x3", "x4", "x5", "x6", "x7", \
    "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18"
#define SYSCALL_NUMBER_REGISTER "x8"
#define SYSCALL_RESULT_REGISTER "x0"
#define SYSCALL_INSTRUCTION     "svc 0"

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "x0", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "x1", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "x2", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "x3", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "x4", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "x5", value)
#define SYSCALL_SET_ARG_7(value) static_assert(false, "Too many arguments for this architecture.")

#endif
