#ifndef _SYSCALL_ABI_H
#define _SYSCALL_ABI_H

/*
 * Calling convention for MIPS ABI:
 *
 * syscall number: v0
 * arg1          : a0
 * arg2          : a1
 * arg3          : a2
 * arg4          : a3
 * arg5          : [sp + 16]
 * arg6          : [sp + 20]
 * arg7          : [sp + 24]
 *
 * syscall
 *
 * return value to v0
 */

#error "Syscall ABI not supported for this CPU."

#define SYSCALL_CLOBBERED_REGISTERS "at", "v1", "hi", "lo", \
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"
#define SYSCALL_NUMBER_REGISTER "v0"
#define SYSCALL_RESULT_REGISTER "v0"
#define SYSCALL_INSTRUCTION     "syscall"
#define SYSCALL_PUSH_ARGUMENTS(...)
#define SYSCALL_PUSH_ARGUMENTS(...) \
({  \
    size_t nr_args = SyscallArgumentsLength(__VA_ARGS__); \
    void **stack_args = (void **) __builtin_alloca(sizeof(long) * 8); \
    SYSCALL_ARG_BIND_REGISTER(0, "a0", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, "a1", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, "a2", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, "a3", __VA_ARGS__); \
    if ( nr_args > 4 ) \
        stack_args[4] = reinterpret_cast<void *>(GetArgumentByIndex<4>(__VA_ARGS__)); \
})

#endif
