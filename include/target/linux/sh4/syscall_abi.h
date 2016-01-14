#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for Linux SH4: 
 *
 * syscall number: r3
 * arg1          : r4
 * arg2          : r5
 * arg3          : r6
 * arg4          : r0
 * arg5          : r1
 * arg6          : r2
 *
 * trapa 0x1f
 *
 * return value to r0
 */

#define SYSCALL_CLOBBERED_REGISTERS "t"
#define SYSCALL_NUMBER_REGISTER "r3"
#define SYSCALL_RESULT_REGISTER "r0"
#define SYSCALL_INSTRUCTION     "trapa #31"

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "r4", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "r5", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "r6", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "r0", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "r1", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "r2", value)
#define SYSCALL_SET_ARG_7(value) static_assert(false, "Too many arguments for this architecture.")

#endif
