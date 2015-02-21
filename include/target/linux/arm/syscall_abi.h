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
#define SYSCALL_PUSH_ARGUMENTS(...) \
    SYSCALL_ARG_BIND_REGISTER(0, "r0", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, "r1", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, "r2", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, "r3", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(4, "r4", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(5, "r5", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(6, "r6", __VA_ARGS__);

#endif
