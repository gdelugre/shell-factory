#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for Linux PowerPC:
 *
 * syscall number: r0
 * arg1          : r3
 * arg2          : r4
 * arg3          : r5
 * arg4          : r6
 * arg5          : r7
 * arg6          : r8
 *
 * sc
 *
 * return value to r3
 */

#define SYSCALL_CLOBBERED_REGISTERS "cr0", "ctr"
#define SYSCALL_NUMBER_REGISTER "r0"
#define SYSCALL_RESULT_REGISTER "r3"
#define SYSCALL_INSTRUCTION     "sc"

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "r3", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "r4", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "r5", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "r6", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "r7", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "r8", value)

#endif
