#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for ARM EABI: 
 *
 * syscall number: r7
 * arg1          : r0
 * arg2          : r1
 * arg3          : r2
 * arg4          : r3
 * arg5          : r4
 * arg6          : r5
 * arg7          : r6
 *
 * swi 0x0
 *
 * return value to r0
 */

#define SYSCALL_CLOBBERED_REGISTERS "lr"
#define SYSCALL_NUMBER_REGISTER "r7"
#define SYSCALL_RESULT_REGISTER "r0"
#define SYSCALL_INSTRUCTION     "swi 0x0"

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "r0", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "r1", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "r2", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "r3", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "r4", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "r5", value)
#define SYSCALL_SET_ARG_7(value) SYSCALL_ARG_BIND_REGISTER(7, "r6", value)

#endif
